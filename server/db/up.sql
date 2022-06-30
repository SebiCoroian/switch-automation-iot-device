CREATE TABLE `devices` (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    device_id VARCHAR(255),
    name VARCHAR(255),
    state VARCHAR(255),

    PRIMARY KEY (id)
);