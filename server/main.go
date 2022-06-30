package main

import (
	"context"
	"database/sql"
	"errors"
	"fmt"
	"net"
	"net/http"
	"strings"
	"time"

	"github.com/doug-martin/goqu/v9"
	_ "github.com/doug-martin/goqu/v9/dialect/mysql"
	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/gin-gonic/gin"
	"github.com/go-sql-driver/mysql"
)

const (
	// address of the mqtt broker
	MQTT_BROKER = "tcp://localhost:1883"
	// mqtt username and password
	MQTT_USERNAME = ""
	MQTT_PASSWORD = ""
	// client id of this webserver - for identification purposes
	MQTT_CLIENT_ID = "graduation-thesis-webserver"

	MYSQL_USER     = "root"
	MYSQL_PASSWORD = "Password1234."
	MYSQL_HOST     = "127.0.0.1"
	MYSQL_PORT     = "3306"
	MYSQL_DB       = "graduation"
)

var (
	// device push angle - 180 degrees
	PUSH_ANGLE = fmt.Sprintf("%d", 130)
	// device pull angle - 0 degress
	PULL_ANGLE = fmt.Sprintf("%d", 0)

	MYSQL_URL = &mysql.Config{
		User:   MYSQL_USER,
		Passwd: MYSQL_PASSWORD,
		Net:    "tcp",
		Addr:   net.JoinHostPort(MYSQL_HOST, MYSQL_PORT),
		DBName: MYSQL_DB,
	}
)

type Device struct {
	ID       uint64 `json:"-" db:"id,skipinsert"`
	DeviceID string `json:"id" db:"device_id"`
	Name     string `json:"name" db:"name"`
	State    string `json:"state" db:"state"`
}

func main() {
	// create mqtt client options
	// set mqtt server (broker) and a client id for easy identification
	// set mqtt username and password
	opts := mqtt.NewClientOptions()
	opts.AddBroker(MQTT_BROKER)
	opts.SetClientID(MQTT_CLIENT_ID)
	opts.SetUsername(MQTT_USERNAME)
	opts.SetPassword(MQTT_PASSWORD)
	opts.SetAutoReconnect(true)
	opts.SetOrderMatters(false)
	opts.SetCleanSession(false)
	// keep connections alive for 2 seconds for performance reasons
	opts.SetKeepAlive(2 * time.Second)
	// ping the second every second to maintain the connection
	opts.SetPingTimeout(1 * time.Second)
	// create the actual client
	mqttClient := mqtt.NewClient(opts)
	defer mqttClient.Disconnect(5000)

	fmt.Println("connecting to mqtt")

	// attempt to connect to the mqtt broker
	if token := mqttClient.Connect(); token.Wait() && token.Error() != nil {
		// if we can't connect, exit now
		panic(token.Error())
	}

	fmt.Println("connected to mqtt")

	// connect to the database
	mysqlDb, err := sql.Open("mysql", MYSQL_URL.FormatDSN())
	if err != nil {
		panic(err)
	}
	defer mysqlDb.Close()
	// if the server can't connected to the database, exit now
	if err := mysqlDb.Ping(); err != nil {
		panic(err)
	}
	db := goqu.New("mysql", mysqlDb)

	// subscribe to the connected devices topic to listen for new devices
	t := mqttClient.Subscribe("connected-device", 2, func(c mqtt.Client, m mqtt.Message) {
		fmt.Println("received autodiscovery message:", string(m.Payload()))
		payload := strings.Split(string(m.Payload()), ";")
		deviceID := payload[0]
		deviceName := payload[1]
		go func() {
			_, err := db.Insert(goqu.I("devices")).Rows(Device{
				DeviceID: deviceID,
				Name:     deviceName,
				State:    "default",
			}).Executor().ExecContext(context.Background())
			if err != nil {
				fmt.Printf("failed to add new device: %v\n", err)
			}
		}()
	})
	go func() {
		<-t.Done()
		if t.Error() != nil {
			panic(t.Error())
		}
	}()

	// create web router
	router := gin.Default()

	// register an endpoint for sending a push command
	router.GET("/push/:device_id", func(c *gin.Context) {
		fmt.Printf("device %s -- push\n", c.Param("device_id"))
		// publish a message to the topic with the device id name that contains the push angle
		token := mqttClient.Publish(c.Param("device_id"), 0, false, PUSH_ANGLE)
		// if we failed to publish the message return an error
		if token.Wait() && token.Error() != nil {
			c.Error(token.Error())
			c.Status(http.StatusInternalServerError)
			return
		}
		// update device state in db
		result, err := db.
			Update(goqu.I("devices")).
			Set(goqu.Record{"state": "push"}).
			Where(goqu.I("device_id").Eq(c.Param("device_id"))).
			Executor().
			ExecContext(c.Request.Context())
		// if we have an error return it now
		if err != nil {
			c.Error(err)
			c.Status(http.StatusInternalServerError)
			return
		}
		rowsAffected, err := result.RowsAffected()
		// if we have an error return it now
		if err != nil {
			c.Error(err)
			c.Status(http.StatusInternalServerError)
			return
		}
		// we updated no rows, one row should have been updated
		if rowsAffected != 1 {
			c.Error(errors.New("no such device"))
			c.Status(http.StatusInternalServerError)
			return
		}
		// otherwise return HTTP 200 OK
		c.Status(200)

	})
	// register an endpoint for sending a pull command
	router.GET("/pull/:device_id", func(c *gin.Context) {
		fmt.Printf("device %s -- pull\n", c.Param("device_id"))
		// publish a message to the topic with the device id name that contains the pull angle
		token := mqttClient.Publish(c.Param("device_id"), 0, false, PULL_ANGLE)
		// if we failed to publish the message return an error
		if token.Wait() && token.Error() != nil {
			c.Error(token.Error())
			c.Status(http.StatusInternalServerError)
			return
		}
		// update device state in db
		result, err := db.
			Update(goqu.I("devices")).
			Set(goqu.Record{"state": "pull"}).
			Where(goqu.I("device_id").Eq(c.Param("device_id"))).
			Executor().
			ExecContext(c.Request.Context())
		// if we have an error return it now
		if err != nil {
			c.Error(err)
			c.Status(http.StatusInternalServerError)
			return
		}
		rowsAffected, err := result.RowsAffected()
		// if we have an error return it now
		if err != nil {
			c.Error(err)
			c.Status(http.StatusInternalServerError)
			return
		}
		// we updated no rows, one row should have been updated
		if rowsAffected != 1 {
			c.Error(errors.New("no such device"))
			c.Status(http.StatusInternalServerError)
			return
		}
		// otherwise return HTTP 200 OK
		c.Status(200)
	})
	// register endpoint that fetches list of devices
	router.GET("/devices", func(c *gin.Context) {
		devices := make([]Device, 0)
		// execute query that fetches all devices
		err := db.
			From(goqu.I("devices")).
			ScanStructsContext(c.Request.Context(), &devices)
		// if we get an error, return it now
		if err != nil {
			c.Error(err)
			c.Status(http.StatusInternalServerError)
			return
		}
		// otherwise return a JSON that contains the list of devices
		c.JSON(http.StatusOK, devices)
	})

	fmt.Println("running")

	// run the web server
	http.ListenAndServe(":8888", router)
}
