# SknRanger
Homie ESP8266 Node developed using Automaton State Machines and VL53L1x

<a href="https://homieiot.github.io/">
  <img src="https://homieiot.github.io/img/works-with-homie.png" alt="works with MQTT Homie">
</a>

## Features
* OpenHab3 RollerShutter Type
* Homie/SknGarageDoor/SknRanger/Position/set [ 0:100 ]
* Homie/SknGarageDoor/SknRanger/State [ STOPPED | MOVING_UP | MOVING_DOWN | MOVING_POS | LEARN_UP | LEARN_DOWN ]
* Homie/SknGarageDoor/SknRanger/Service [ AUTO_LEARN_UP | AUTO_LEARN_DOWN | REBOOT ]
* Measures door position in mm, translate to 0:100 range; with 0 considered UP, and 100 Down
* Auto Learn stores min or max positions in EEPROM via Preferences(SPIFFS); basis for 0:100 range translation.

## Components
* [Wemos D1 Mini ESP8266](https://www.amazon.com/MELIFE-Development-Wireless-Internet-MicroPython/dp/B08H1YRN4M/ref=sr_1_10?crid=2Y9PEH0OFTXPL&keywords=memos+d1+mini+esp8266&qid=1663331796&s=electronics&sprefix=memos+d1+mini+esp8266%2Celectronics%2C84&sr=1-10)
* [VL53L1x Time of Flight Sensor](https://www.amazon.com/DWEII-Measurement-Extension-Compatible-Raspberry/dp/B09V4DS888/ref=sr_1_4_sspa?crid=1SZRJKYL8C1IB&keywords=vl53l1x+tof+sensor+module&qid=1663331559&sprefix=vl53l1%2Caps%2C89&sr=8-4-spons&psc=1)
* [5v Relay Board Relay Module 1 Channel Opto-Isolated High or Low Level Trigger](https://www.amazon.com/AOICRIE-Optocoupler-Isolation-Compatible-Development/dp/B08C71QL65/ref=sr_1_6?crid=3GQ5B5O0AT995&keywords=HiLetgo+2pcs+5V+One+Channel+Relay+Module+Relay+Switch+with+OPTO+Isolation+High+Low+Level+Trigger&qid=1663330959&s=electronics&sprefix=hiletgo+2pcs+5v+one+channel+relay+module+relay+switch+with+opto+isolation+high+low+level+trigger%2Celectronics%2C109&sr=1-6)

### Frameworks: 
* [PlatformIO(Arduino)](https://platformio.org)
* [Homie ESP8266/32 v3](https://github.com/homieiot/homie-esp8266)

### Garage Door Travel
* Full travel:        86.5" or 2198 mm
* Device mount point: 13.0" or  330 mm
* Maximum range:               2528 mm
* Compile option to disable relay:  SKN_DISABLE_RELAY

### Wiring Pin
    Module
        ESP8266EX 

    VL53L1x (5VDC)
        13 DataReady 
        5 SCL
        4 SDA

## Homie Config: data/homie/config.json
```
{
  "name": "Garage Door",
  "device_id": "SknGarageDoor",
  "device_stats_interval": 900,  
  "wifi": {
    "ssid": "<wifi-host>",
    "password": "<wifi-password>"
  },
  "mqtt": {
    "host": "<mqtt-hostname-or-ip>",
    "port": 1883,
	"base_topic": "sknSensors/",
    "auth": true,
    "username": "<mqtt-username>",
    "password": "<mqtt-password>"
  },
  "ota": {
    "enabled": true
  },
  "settings": {
  }
}
```

### ESP8266 Console Log
```
{} Stored configuration
  • Hardware device ID: 84f3eb0c386f
  • Device ID: SknRangerD1
  • Name: Garage Door Position Monitor
  • Device Stats Interval: 300 sec
  • Wi-Fi: 
    ◦ SSID: SFNSS1-24G
    ◦ Password not shown
  • MQTT: 
    ◦ Host: openhabianpi.local.skoona.net
    ◦ Port: 1883
    ◦ Base topic: sknSensors/
    ◦ Auth? yes
    ◦ Username: openhabian
    ◦ Password not shown
  • OTA: 
    ◦ Enabled? yes
✖ Node: Garage Door Position Homie setup() Complete
↕ Attempting to connect to Wi-Fi...
✔ Wi-Fi connected, IP: 10.100.1.239
Triggering WIFI_CONNECTED event...
↕ Attempting to connect to MQTT...
Sending initial information...
✔ MQTT ready
Triggering MQTT_READY event...
MQTT connected
 ✖  SknLoxRanger initialization starting.
📢 Calling broadcast handler...
Received broadcast level alert: Anyone can view broadcasts
📢 Calling broadcast handler...
Received broadcast level LWT: skoona-iot-hub going offLine!
 〽  limitsRestore(True) 	min: 235 	max: 2528
 〽  Exited initialize sensor!
 〽 Medium distance mode accepted.
 〽 250ms timing budget accepted.
 ✖  SknLoxRanger initialization Complete.
 ✖  Node: Garage Door Position Ready to operate: DOWN(0) Services Mode: Range Limits, min: 330 mm, max: 2528 mm
Calling setup function...
〽 Sending statistics...
  • Interval: 305s (300s including 5s grace time)
  • Wi-Fi signal quality: 100%
  • Uptime: 2s
  • FreeHeap: 40256b
Sending statistics
 ✖  SknLoxRanger startContinuous(500ms) accepted.
 〽 [      ] range: 1533 mm  avg: 0 mm	status: range valid	raw status: 0	signal: 8.6 MCPS	ambient: 0.2 MCPS	move: MOVING_DOWN
 〽  relativeDistance(0 mm) NOT accepted.
 〽 [      ] range: 1533 mm  avg: 306 mm	status: range valid	raw status: 0	signal: 9.3 MCPS	ambient: 0.2 MCPS	move: MOVING_DOWN
 ✖  relativeDistance(3 %) accepted.
 〽 [     1] range: 1533 mm  avg: 613 mm	status: range valid	raw status: 0	signal: 9.2 MCPS	ambient: 0.3 MCPS	move: MOVING_DOWN
 ✖  relativeDistance(16 %) accepted.
 〽 [     2] range: 1531 mm  avg: 919 mm	status: range valid	raw status: 0	signal: 9.2 MCPS	ambient: 0.3 MCPS	move: MOVING_DOWN
 ✖  relativeDistance(30 %) accepted.
 〽 [     3] range: 1532 mm  avg: 1226 mm	status: range valid	raw status: 0	signal: 9.3 MCPS	ambient: 0.2 MCPS	move: MOVING_DOWN
 ✖  relativeDistance(43 %) accepted.
 〽 [     4] range: 1533 mm  avg: 1532 mm	status: range valid	raw status: 0	signal: 9.3 MCPS	ambient: 0.3 MCPS	move: STOPPED
 ✖  relativeDistance(57 %) accepted.
 〽 [     5] range: 1532 mm  avg: 1532 mm	status: range valid	raw status: 0	signal: 9.2 MCPS	ambient: 0.2 MCPS	move: MOVING_UP
 ✖  relativeDistance(57 %) accepted.
 〽 [     6] range: 1533 mm  avg: 1532 mm	status: range valid	raw status: 0	signal: 9.2 MCPS	ambient: 0.3 MCPS	move: STOPPED
                                              ...
 ✖  relativeDistance(57 %) accepted.
 〽 [    51] range: 1533 mm  avg: 1533 mm	status: range valid	raw status: 0	signal: 9.2 MCPS	ambient: 0.3 MCPS	move: STOPPED
 ✖  relativeDistance(57 %) accepted.
 〽 [    52] range: 1533 mm  avg: 1533 mm	status: range valid	raw status: 0	signal: 9.2 MCPS	ambient: 0.3 MCPS	move: STOPPED
 ✖  relativeDistance(57 %) accepted.
 〽 [    53] range: 1534 mm  avg: 1533 mm	status: range valid	raw status: 0	signal: 9.3 MCPS	ambient: 0.3 MCPS	move: MOVING_UP
 ✖  relativeDistance(57 %) accepted.
 〽 [    54] range: 1533 mm  avg: 1533 mm	status: range valid	raw status: 0	signal: 9.2 MCPS	ambient: 0.3 MCPS	move: MOVING_UP
 ✖  relativeDistance(57 %) accepted.
 〽 [    55] range: 1534 mm  avg: 1533 mm	status: range valid	raw status: 0	signal: 9.3 MCPS	ambient: 0.3 MCPS	move: MOVING_DOWN
 ✖  relativeDistance(57 %) accepted.
 〽 [    56] range: 1534 mm  avg: 1533 mm	status: range valid	raw status: 0	signal: 9.3 MCPS	ambient: 0.3 MCPS	move: MOVING_DOWN
 ✖  relativeDistance(57 %) accepted.
 〽 [    57] range: 1534 mm  avg: 1533 mm	status: range valid	raw status: 0	signal: 9.2 MCPS	ambient: 0.3 MCPS	move: MOVING_DOWN
 ✖  relativeDistance(57 %) accepted.
 〽 [    58] range: 1533 mm  avg: 1533 mm	status: range valid	raw status: 0	signal: 9.2 MCPS	ambient: 0.3 MCPS	move: MOVING_UP
 ✖  relativeDistance(57 %) accepted.
 〽 [    59] range: 1534 mm  avg: 1533 mm	status: range valid	raw status: 0	signal: 9.2 MCPS	ambient: 0.3 MCPS	move: MOVING_DOWN
 ✖  relativeDistance(57 %) accepted.
 〽 [    60] range: 1535 mm  avg: 1533 mm	status: range valid	raw status: 0	signal: 9.3 MCPS	ambient: 0.3 MCPS	move: MOVING_DOWN
 ✖  SknLoxRanger stopContinuous() accepted.
 ✖  relativeDistance(57 %) accepted.
〽 Sending statistics...
  • Interval: 305s (300s including 5s grace time)
  • Wi-Fi signal quality: 100%
  • Uptime: 302s
  • FreeHeap: 39128b
Sending statistics
 ✖  SknLoxRanger startContinuous(500ms) accepted.
 〽 [    61] range: 1533 mm  avg: 1534 mm	status: range valid	raw status: 0	signal: 8.9 MCPS	ambient: 0.3 MCPS	move: MOVING_UP
 ✖  relativeDistance(57 %) accepted.
```

## Contributing

1. Fork it 
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request


## License

The project is available as open source under the terms of the [MIT License](http://opensource.org/licenses/MIT).
