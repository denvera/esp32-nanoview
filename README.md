# ESP32 NanoView
Reads data from a [NanoView](http://www.nanoview.co.za/) and sends it as JSON via MQTT.  
Intended to be useful with Grafana + InfluxDB or similar.


[Protocol details](http://www.nanoview.co.za/protocol.html)

## Wiring
Useful info can be found at: [Hacking the NanoView with a RPi Zero W](http://silico.co.za/blog/2019/04/13/hacking-the-nanoview-with-a-rpi-zero-w/).  
Instead of connecting TX to the RPi via voltage divider (Step 8 in the article linked above), connect it to the ESP32 configured UART RX pin (GPIO 5 as set in `nanoview_uart.c`). Ensure the ground of NanoHub and ESP32 are connected together too.

## ESP32 esp-idf Project Configuration
### Menuconfig
Either run `idf.py menuconfig` and set the WiFi and MQTT parameters (under "MQTT and WiFi Configuration"), or use an include file as below.

### Include File 
Create `nanoview_config.h` in the `main/` directory, and enable the `Use nanoview_config.h instead of below` configuration option from `idf.py menuconfig`

The file content should be as below:

```
#define ESP_WIFI_SSID      "your_wifi_name"
#define ESP_WIFI_PASS      "your_super_secret_wifi_key"
#define ESP_MAXIMUM_RETRY  10

#define BROKER_URL "mqtt://esp32-nanoview:yourpassword@192.168.0.123"
```

## MQTT Setup
Install an MQTT broker (eg: Mosquitto), and create `/etc/mosquitto/mosquitto.conf` with the below or similar.
```port 1883
log_dest syslog
connection_messages true
password_file /etc/mosquitto/pwfile
```

Update/create your password_file (`/etc/mosquitto/pwfile`), eg: `mosquitto_passwd [-c] esp32-nanoview`

Note: `-c` will create/overwrite existing files.

## Telegraf Setup
Telegraf can ingest JSON from the MQTT broker, and store data in InfluxDB. An example configuration is below. InfluxDB v1.x is recommended for now.

Example `/etc/telegraf/telegraf.conf` below:

```
# Below are just the defaults, whatever your distribution sets is probably fine. You likely just need to set up the outputs and inputs sections.
[global_tags]
[agent]
  interval = "10s"
  round_interval = true
  metric_batch_size = 1000
  metric_buffer_limit = 10000
  collection_jitter = "0s"
  flush_interval = "10s"
  flush_jitter = "0s"
  precision = ""
  hostname = ""
  omit_hostname = false

# Recommend using InfluDB v1.x to be able to use the included Grafana queries/dashboard examples.
[[outputs.influxdb]]
  urls = ["http://127.0.0.1:8086"]
   database = "telegraf"

# But you can output to InfluxDB v2 just fine too if you wish
[[outputs.influxdb_v2]]
  urls = ["http://localhost:9999"]
  token = "your_influx_v2_token"
  organization = "your_org"
  bucket = "your_bucket"

# Consume the JSON from your MQTT broker
[[inputs.mqtt_consumer]]
  servers = ["tcp://esp32-nanoview:your_mqtt_password@192.168.0.123:1883"]
  topics = [
    "/esp-mqtt/nanoview"
  ]
  data_format = "json"
  json_name_key = "name"
  tag_keys = ["channel"]
```

## Grafana Setup
Install Grafana and add your InfluxDB v1.x datasource. 
To import the included dashboard, configure the datasource as:

```
Name: InfluxDB1
Query Language: InfluxQL
Database: telegraf (or whatever you set in your telegraf.conf)
```

Import the included `grafana/grafana-example.json` dashboard (Dashboards -> Manage -> Import) to get something like the below.
Select your Influx v1.x datasource and set a name as desired. You'll likely need to update the annotations and field overrides for your own setup.

![Example Dashboard](/grafana/grafana-example.png?raw=true "Example Dashboard")