# esp-data-module
A (more or less) universal data collection system and module for ESP-Arduino boards.

---

## General
The data collection System consists of 3 layers. The first layer consists of ESP-Boards doing measurements or gathering data. At the second layer, there are data collection ESPs. These recieve data via ESPNOW and can then upload it to a database or store it locally. An InfluxDB-Grafana Combination then makes up the third layer, allowing for centralized data management, storage and visualisation.
This three-level system allows for easy optimization where it is needed. Some examples include:
- server level: self-hosting or cloud computing
- data collection level: local storage (e.g. SD-Card module) and/or server upload
- data transmission: general-purpose protocol or custom protocol (polling rate, special datatypes, faster or more secure connection, etc.)

---

## ESP Communication (1 -> 2)
- communication via ESPNOW
- public broadcasting as default (no complicated setup required)
- data is transmitted as struct: Default is float data, but the struct can easily be changed

---

## Data Collection Module and Forwarding/Upload (2 -> 3)
- different options for data collection/forwarding
    - strict forwarding of data
        - fields are sent when they arrive at the module, even if not all fields have new measurements
        - usually works well with Influx/Grafana, but could pose problems when using SQL for querying
        - -> easiest option if simple visualisation is the goal
    - caching until all fields are filled
        - module caches all fields, until every field has received a new value
        - avoid inconsitencies with SQL
        - -> well-suited for more complex SQL querying/ visualisation
    - independently from the above: local data storage
        - e.g. additional SD-Card module
        - extra layer of safety
        - local time must be obtained, either through WiFi-API or RTC-module
        
---

## Server-side
- lorem ipsum
