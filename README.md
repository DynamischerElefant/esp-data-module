# esp-data-module
A (more or less) universal data collection system and module for ESP-Arduino boards.


## General
The data collection System consists of 3 layers. The first layer consists of ESP-Boards doing measurements or gathering data. At the second layer, there are data collection ESPs. These recieve data via ESPNOW and can then upload it to a database or store it locally. An InfluxDB-Grafana Combination then makes up the third layer, allowing for centralized data management, storage and visualisation.
This three-level system allows for easy optimization where it is needed. Some examples include:
- server level: self-hosting or cloud computing
- data collection level: local storage (e.g. SD-Card module) and/or server upload
- data transmission: general-purpose protocol or custom protocol (polling rate, special datatypes, faster or more secure connection, etc.)


## ESP Communication (1 -> 2)
- communication via ESPNOW
- public broadcasting as default (no complicated setup required)
- data is transmitted as struct: Default is float data, but the struct can easily be changed
- the struct contains the measurements and their lables, Caution: every measurement label (name) can be used **only once**
- ESPNOW in combination with WiFi functionality makes receiver unreliable (potential fix: BLE?)

## Data Collection Module and Forwarding/Upload (2 -> 3)
- different options for data collection/forwarding
    - strict forwarding of data (default)
        - fields are sent when they arrive at the module, even if not all fields have new measurements
        - some values sent will not be fresh, so there will be duplicates (but no empty slots) in the database
        - -> easiest option if simple visualisation is the goal
    - caching until all fields are filled
        - module caches all fields, until every field has received a new value
        - avoid inconsitencies with SQL
        - -> well-suited for more complex SQL querying/ visualisation
    - independently from the above: local data storage
        - e.g. additional SD-Card module
        - extra layer of safety
        - local time must be obtained, either through WiFi-API or RTC-module
        

## Server-side
- run InfluxDB and Grafana
    - InfluxDB3.x for long term use (newer, better long-term support)
    - InfluxDB2.x if you are short on memory/ RAM or want a fast out-of-the-box working setup

## Setup
### Sender (sensor-side)
- in your sensor-esp, insert the code from `sender.ino` somewhere in your sender code
- `sender.ino` is an example for sending data, after inserting the code, you only have to change a few things
    - set dataLabels to whatever your measurements are called (max 4 letters)
    - make sure to delete the default values and to dynamically set the values using data[i] = measurement for every measurement

### Receiver (data-esp)
- setup an esp with the `receiver.ino` code
- things to change in the code
    - change the wifi-credentials to those of your wifi (WIFI_SSID, WIFI_PASSWORD)
    - setup your Influx connection (INFLUXDB_URL, INFLUXDB_TOKEN, INFLUXDB_ORG, INFLUXDB_BUCKET)
    - change your timezone info and your device name (TZ_INFO, DEVICE)
    - declare your sensor point name (recommended: use different point names for different data-esps)
    - change or add sensor tags (helpful for identification of data source)

### Server
- install an InfluxDB version of your choice [Installation Guide for InfluxDB3](https://docs.influxdata.com/influxdb3/core/install/), [Installation Guido for InfluxDB2](https://docs.influxdata.com/influxdb/v2/install/), [Installation and Setup for InfluxDB2 (Ubuntu, unofficial)](https://de.linux-console.net/?p=29544)
    - -> Database Writes for InfluxDB2 now possible over HTTP (no encryption)
    - additional notes for InfluxDB2
        - a tutorial for Seting up Influx2 is provided inside the Web GUI (standard access: yourserverip:8086)
        - when setting the url inside the arduino script, make sure to also include the port (for self hosting), e.g. `#define INFLUXDB_URL "http://yourserverip:8086"`
        - use the `influx_test.ino` script to test if your InfluxDB instance is working
    - additional notes for InfluxDB3
        - InfluxDB3 doesn't expose the HTTP API per default, so port forwarding/reverse proxy is necessary
        - InfluxDB3 does not share the same endpoints as INFLUXDB2, so third-party-libraries like the Arduino Influx library won't work. Instead, manually conduct HTTP requests to the corresponding endpoints.
- install Grafana OSS ([Installation Guide for Ubuntu/Debian](https://grafana.com/docs/grafana/latest/setup-grafana/installation/debian/))
- setup Grafana ([official Sign-In Guide](https://grafana.com/docs/grafana/latest/setup-grafana/sign-in-to-grafana/))
- connect Influx and Grafana [official Grafana-Influx Guide](https://grafana.com/docs/grafana/latest/getting-started/get-started-grafana-influxdb/)
    - **Caution:** Make sure you use different API Tokens for each use case, e.g. Grafana or Arduinos. Else you might encounter issues with autorisation
    - create a new Dashboard with the values from your esp
