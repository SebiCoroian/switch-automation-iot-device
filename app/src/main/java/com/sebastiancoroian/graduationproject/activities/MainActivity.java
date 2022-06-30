package com.sebastiancoroian.graduationproject.activities;

import android.os.Bundle;
import android.util.Log;
import android.widget.ListView;

import androidx.appcompat.app.AppCompatActivity;

import com.google.gson.Gson;
import com.sebastiancoroian.graduationproject.adapters.DeviceListAdapter;
import com.sebastiancoroian.graduationproject.services.DeviceService;
import com.sebastiancoroian.graduationproject.R;

import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.d(TAG, "creating http service utilities");
        final Gson gson = new Gson();
        final Retrofit retrofit = new Retrofit.Builder()
                .baseUrl(getString(R.string.device_service_base_url))
                .addConverterFactory(GsonConverterFactory.create(gson))
                .build();

        Log.d(TAG, "creating device service");
        final DeviceService deviceService = retrofit.create(DeviceService.class);

        Log.d(TAG, "creating device list adapter");
        final DeviceListAdapter deviceListAdapter = new DeviceListAdapter(deviceService);

        Log.d(TAG, "creating device list");
        final ListView deviceList = findViewById(R.id.device_list);
        deviceList.setAdapter(deviceListAdapter);

        Log.i(TAG, "initialized main activity");
    }
}