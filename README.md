# SknRanger
Homie ESP8266 Node developed using Automaton State Machines and VL53L1x

<a href="https://homieiot.github.io/">
  <img src="https://homieiot.github.io/img/works-with-homie.png" alt="works with MQTT Homie">
</a>

## Features
* OpenHab3 RollerShutter Type
* Homie/SknGarageDoor/GarageDoor/Position/set [ 0:100 | UP | DOWN ]
* Homie/SknGarageDoor/GarageDoor/State [ STOPPED | MOVING_UP | UP | MOVING_DOWN | DOWN | MOVING_POS | LEARN_UP | LEARN_DOWN ]
* Homie/SknGarageDoor/GarageDoor/Service [ AUTO_LEARN_UP | AUTO_LEARN_DOWN | REBOOT ]
* Measures door position in mm, translate to 0:100 range; with 0 considered UP, and 100 Down
* Operates Door Relay with normal or short click hold times; meant to run fully or stop door.
* Auto Learn stores min or max positions in EEPROM via Preferences(SPIFFS); basis for 0:100 range translation.

## Components
* [Wemos D1 Mini ESP8266](https://www.amazon.com/MELIFE-Development-Wireless-Internet-MicroPython/dp/B08H1YRN4M/ref=sr_1_10?crid=2Y9PEH0OFTXPL&keywords=memos+d1+mini+esp8266&qid=1663331796&s=electronics&sprefix=memos+d1+mini+esp8266%2Celectronics%2C84&sr=1-10)
* [VL53L1x Time of Flight Sensor](https://www.amazon.com/DWEII-Measurement-Extension-Compatible-Raspberry/dp/B09V4DS888/ref=sr_1_4_sspa?crid=1SZRJKYL8C1IB&keywords=vl53l1x+tof+sensor+module&qid=1663331559&sprefix=vl53l1%2Caps%2C89&sr=8-4-spons&psc=1)
* [5v Relay Board Relay Module 1 Channel Opto-Isolated High or Low Level Trigger](https://www.amazon.com/AOICRIE-Optocoupler-Isolation-Compatible-Development/dp/B08C71QL65/ref=sr_1_6?crid=3GQ5B5O0AT995&keywords=HiLetgo+2pcs+5V+One+Channel+Relay+Module+Relay+Switch+with+OPTO+Isolation+High+Low+Level+Trigger&qid=1663330959&s=electronics&sprefix=hiletgo+2pcs+5v+one+channel+relay+module+relay+switch+with+opto+isolation+high+low+level+trigger%2Celectronics%2C109&sr=1-6)

### Frameworks: 
* [PlatformIO(Arduino)](https://platformio.org)
* [Homie ESP8266/32 v3](https://github.com/homieiot/homie-esp8266)
* [Automaton Reactive State Machine)](https://github.com/tinkerspy/Automaton)

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
    
    Relay (5VDC)
       12 Activate-HIGH, or pin that is not HIGH on boot


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

### OpenHab3 Console Log
```
    2022-09-14 20:57:09.780 [INFO ] [openhab.event.ItemCommandEvent      ] - Item 'GarageDoor_Position' received command UP
    2022-09-14 20:57:09.788 [INFO ] [penhab.event.ItemStatePredictedEvent] - Item 'GarageDoor_Position' predicted to become UP
    2022-09-14 20:57:13.870 [INFO ] [openhab.event.ItemCommandEvent      ] - Item 'GarageDoor_Position' received command DOWN
    2022-09-14 20:57:13.878 [INFO ] [penhab.event.ItemStatePredictedEvent] - Item 'GarageDoor_Position' predicted to become DOWN
    2022-09-14 20:57:13.886 [INFO ] [openhab.event.ItemStateChangedEvent ] - Item 'GarageDoor_Position' changed from 0 to 100
    2022-09-14 20:57:17.836 [INFO ] [openhab.event.ItemCommandEvent      ] - Item 'GarageDoor_Position' received command STOP
```

### ESP8266 Console Log
```
    🔌 Booting into normal mode 🔌
    {} Stored configuration
      • Hardware device ID: c45bbe64ee02
      • Device ID: SknGarageDoor
      • Name: Garage Door Automaton
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
    〽 Node: Garage Door SknGarageDoor::setup() 
    ↕ Attempting to connect to Wi-Fi...
    ✔ Wi-Fi connected, IP: 10.100.1.113
    Triggering WIFI_CONNECTED event...
    ↕ Attempting to connect to MQTT...
    Sending initial information...
    ✔ MQTT ready
    Triggering MQTT_READY event...
    MQTT connected
    📢 Calling broadcast handler...
    Received broadcast level alert: Anyone can view broadcasts
    📢 Calling broadcast handler...
    Received broadcast level LWT: skoona-iot-hub going offLine!
    〽 Node: Garage Door Auto Learn Limits Restoring:[True] up=329, down=2410
    〽 Medium distance mode accepted.
    〽 300ms timing budget accepted.
    ✖  SknLoxRanger initialization Complete.
    〽 range: 11 mm avgerage: 1 mm,	status: range valid	raw: 0	signal: 26.5 MCPS	ambient: 0.0 MCPS
    〽 range: 13 mm avgerage: 4 mm,	status: range valid	raw: 0	signal: 13.6 MCPS	ambient: 0.0 MCPS
    〽 range: 13 mm avgerage: 6 mm,	status: range valid	raw: 0	signal: 22.8 MCPS	ambient: 0.0 MCPS
    〽 range: 13 mm avgerage: 8 mm,	status: range valid	raw: 0	signal: 13.6 MCPS	ambient: 0.0 MCPS
    SknAtmDoor::setDoorPosition(5:-1) Position:0, Moving:STOPPED, ePos:STOPPED, sReq:STOPPED, sCur:ELSE, sNext:STOPPED, chgDir:False, A:0, B:0
    10484 Switch DOOR@3FFEF040 from *NONE* to STOPPED on *NONE* (1 cycles in 10484 ms)
    〽 Node: Garage Door Ready to operate: STOPPED(0) Auto Learn: Auto Learn Range, Up 330 mm, Down 2528 mm
    Calling setup function...
    〽 Sending statistics...
      • Interval: 305s (300s including 5s grace time)
      • Wi-Fi signal quality: 100%
      • Uptime: 11s
      • FreeHeap: 39840b
    Sending statistics
    ✖  〽 handleInput -> property 'Position' value=up
    27843 Switch DOOR@3FFEF040 from STOPPED to MOVING_UP on EVT_UP (358162 cycles in 17357 ms)
    28233 Switch DOOR@3FFEF040 from MOVING_UP to UP on EVT_POS_REACHED (1 cycles in 389 ms)
    〽 range: 327 mm avgerage: 62 mm,	status: range valid	raw: 0	signal: 1.7 MCPS	ambient: 0.1 MCPS
    SknAtmDoor::setDoorPosition(6:1) Position:0, Moving:STOPPED, ePos:STOPPED, sReq:MOVING_UP, sCur:UP, sNext:ELSE, chgDir:True, A:0, B:0
    ✖  〽 handleInput -> property 'Position' value=down
    39713 Switch DOOR@3FFEF040 from UP to MOVING_DOWN on EVT_DOWN (217707 cycles in 11479 ms)
    〽 range: 1143 mm avgerage: 253 mm,	status: range valid	raw: 0	signal: 9.4 MCPS	ambient: 4.2 MCPS
    SknAtmDoor::setDoorPosition(7:2) Position:39, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_DOWN, sCur:MOVING_DOWN, sNext:ELSE, chgDir:True, A:0, B:39
    〽 range: 1510 mm avgerage: 503 mm,	status: range valid	raw: 0	signal: 9.3 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(8:3) Position:57, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_DOWN, sCur:MOVING_DOWN, sNext:ELSE, chgDir:True, A:0, B:57
    〽 range: 1560 mm avgerage: 761 mm,	status: range valid	raw: 0	signal: 8.1 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(9:4) Position:59, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_DOWN, sCur:MOVING_DOWN, sNext:ELSE, chgDir:True, A:0, B:59
    〽 range: 1710 mm avgerage: 0 mm,	status: wrap target fail	raw: 7	signal: 5.9 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(10:5) Position:66, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_DOWN, sCur:MOVING_DOWN, sNext:ELSE, chgDir:True, A:0, B:66
    〽 range: 1955 mm avgerage: 1109 mm,	status: range valid	raw: 0	signal: 4.2 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(11:6) Position:78, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_DOWN, sCur:MOVING_DOWN, sNext:ELSE, chgDir:True, A:0, B:78
    〽 range: 1818 mm avgerage: 1410 mm,	status: range valid	raw: 0	signal: 5.3 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(12:7) Position:72, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_DOWN, sCur:MOVING_DOWN, sNext:ELSE, chgDir:True, A:39, B:72
    〽 range: 1588 mm avgerage: 1620 mm,	status: range valid	raw: 0	signal: 7.7 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(13:8) Position:60, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_DOWN, sCur:MOVING_DOWN, sNext:ELSE, chgDir:True, A:57, B:60
    〽 range: 1475 mm avgerage: 1676 mm,	status: range valid	raw: 0	signal: 9.5 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(14:9) Position:55, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_DOWN, sCur:MOVING_DOWN, sNext:ELSE, chgDir:True, A:59, B:55
    〽 range: 1561 mm avgerage: 1684 mm,	status: range valid	raw: 0	signal: 8.0 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(15:10) Position:59, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_DOWN, sCur:MOVING_DOWN, sNext:ELSE, chgDir:True, A:66, B:59
    〽 range: 1785 mm avgerage: 0 mm,	status: wrap target fail	raw: 7	signal: 5.1 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(16:11) Position:70, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_DOWN, sCur:MOVING_DOWN, sNext:ELSE, chgDir:True, A:78, B:70
    〽 range: 2131 mm avgerage: 0 mm,	status: wrap target fail	raw: 7	signal: 3.0 MCPS	ambient: 0.3 MCPS
    SknAtmDoor::setDoorPosition(17:12) Position:87, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_DOWN, sCur:MOVING_DOWN, sNext:ELSE, chgDir:True, A:72, B:87
    〽 range: 2873 mm avgerage: 1973 mm,	status: range valid	raw: 0	signal: 1.9 MCPS	ambient: 0.2 MCPS
    SknAtmDoor::setDoorPosition(18:13) Position:100, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_DOWN, sCur:MOVING_DOWN, sNext:ELSE, chgDir:True, A:60, B:100
    50434 Switch DOOR@3FFEF040 from MOVING_DOWN to DOWN on EVT_POS_REACHED (192493 cycles in 10719 ms)
    ✖  〽 handleInput -> property 'Position' value=up
    61740 Switch DOOR@3FFEF040 from DOWN to MOVING_UP on EVT_UP (213986 cycles in 11299 ms)
    〽 range: 1457 mm avgerage: 1890 mm,	status: range valid	raw: 0	signal: 9.8 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(19:14) Position:54, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:55, B:54
    〽 range: 1464 mm avgerage: 1831 mm,	status: range valid	raw: 0	signal: 9.7 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(20:15) Position:55, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:59, B:55
    〽 range: 1462 mm avgerage: 1810 mm,	status: range valid	raw: 0	signal: 9.7 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(21:16) Position:54, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:70, B:54
    〽 range: 1462 mm avgerage: 1808 mm,	status: range valid	raw: 0	signal: 9.7 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(22:17) Position:54, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:87, B:54
    〽 range: 1467 mm avgerage: 1697 mm,	status: range valid	raw: 0	signal: 9.6 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(23:18) Position:55, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:100, B:55
    〽 range: 1479 mm avgerage: 1465 mm,	status: range valid	raw: 0	signal: 9.5 MCPS	ambient: 0.4 MCPS
    ✖✖✖ SknAtmDoor::relayChangeDirection() ep=55, rp=0, ePos:STOPPED, eReq:MOVING_UP, curr:MOVING_UP
    SknAtmDoor::setDoorPosition(24:0) Position:55, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:54, B:55
    〽 range: 1515 mm avgerage: 1474 mm,	status: range valid	raw: 0	signal: 8.9 MCPS	ambient: 0.4 MCPS
    SknAtmDoor::setDoorPosition(25:1) Position:57, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:55, B:57
    〽 range: 1567 mm avgerage: 1492 mm,	status: range valid	raw: 0	signal: 8.2 MCPS	ambient: 0.3 MCPS
    SknAtmDoor::setDoorPosition(26:2) Position:59, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:54, B:59
    〽 range: 1627 mm avgerage: 1519 mm,	status: range valid	raw: 0	signal: 7.2 MCPS	ambient: 0.3 MCPS
    SknAtmDoor::setDoorPosition(27:3) Position:62, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:54, B:62
    〽 range: 1647 mm avgerage: 1550 mm,	status: range valid	raw: 0	signal: 5.5 MCPS	ambient: 0.3 MCPS
    SknAtmDoor::setDoorPosition(28:4) Position:63, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:55, B:63
    〽 range: 1513 mm avgerage: 1558 mm,	status: range valid	raw: 0	signal: 4.9 MCPS	ambient: 0.3 MCPS
    SknAtmDoor::setDoorPosition(29:5) Position:57, Moving:MOVING_DOWN, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:55, B:57
    〽 range: 1406 mm avgerage: 1545 mm,	status: range valid	raw: 0	signal: 4.4 MCPS	ambient: 0.3 MCPS
    SknAtmDoor::setDoorPosition(30:6) Position:52, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:57, B:52
    〽 range: 1279 mm avgerage: 1506 mm,	status: range valid	raw: 0	signal: 3.8 MCPS	ambient: 0.2 MCPS
    SknAtmDoor::setDoorPosition(31:7) Position:46, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:59, B:46
    〽 range: 1142 mm avgerage: 1435 mm,	status: range valid	raw: 0	signal: 3.2 MCPS	ambient: 0.2 MCPS
    SknAtmDoor::setDoorPosition(32:8) Position:39, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:62, B:39
    〽 range: 1019 mm avgerage: 1334 mm,	status: range valid	raw: 0	signal: 2.9 MCPS	ambient: 0.2 MCPS
    SknAtmDoor::setDoorPosition(33:9) Position:33, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:63, B:33
    〽 range: 509 mm avgerage: 1144 mm,	status: range valid	raw: 0	signal: 2.6 MCPS	ambient: 0.2 MCPS
    SknAtmDoor::setDoorPosition(34:10) Position:9, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:57, B:9
    〽 range: 411 mm avgerage: 961 mm,	status: range valid	raw: 0	signal: 3.1 MCPS	ambient: 0.2 MCPS
    SknAtmDoor::setDoorPosition(35:11) Position:4, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:52, B:4
    〽 range: 361 mm avgerage: 786 mm,	status: range valid	raw: 0	signal: 4.7 MCPS	ambient: 0.2 MCPS
    SknAtmDoor::setDoorPosition(36:12) Position:2, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:46, B:2
    〽 range: 319 mm avgerage: 626 mm,	status: range valid	raw: 0	signal: 9.9 MCPS	ambient: 0.1 MCPS
    SknAtmDoor::setDoorPosition(37:13) Position:0, Moving:MOVING_UP, ePos:STOPPED, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:True, A:39, B:0
    79063 Switch DOOR@3FFEF040 from MOVING_UP to UP on EVT_POS_REACHED (307357 cycles in 17322 ms)
```

## Contributing

1. Fork it 
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request


## License

The project is available as open source under the terms of the [MIT License](http://opensource.org/licenses/MIT).
