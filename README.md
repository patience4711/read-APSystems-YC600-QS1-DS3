# read APS inverters
This project is intended for reading APS Systems inverters. The program can pair and poll YC600 and QS1 inverters, up to 9 peaces. The read values are displayed on a web page and sent via mosquitto in a Json format.
The program has a lot of smart features. All settings can be done via the webinterface. It has a console that can be used for debugging and sending/receiving zigbee messages.

See it in action on youtube: https://youtu.be/7ZOAcrYXxbM

## features
- Easy wifi config portal
- Easy add, delete and pair inverters
- automatic polling or on demand via mqtt or http
- data can be requested via http and mosquitto
- There are 2 different json formats, a special one for domoticz
- Fast asyc webserver
- a very smart on-line console to send commands and debug
- Smart timekeeping
- A lot of system info on the webpage

## compile this sketch
Use arduino ide with the esp822 version 2.7.1 installed under boardmanager. The ota updates won't work with other versons.

Or use the binary file ESP-ECU-v6_12 to flash directly on your ESP device.

## the hardware
It is nothing more than an esp device like nodemcu or its relatives and a prepared cc2530, cc2531 zigbee module. And a powersupply.
The zigbeemodule should be flashed with a firmware that can be found here https://github.com/Koenkk/zigbee2mqtt/issues/4221. Here you can find much more information on this project.
the wiring:
### cc2530 -> ESP 
-  p2   -> d8
-  p3   -> d7
-  vcc  -> 3.3v
-  gnd  -> gnd
-  rst  -> d5

![minhw](https://user-images.githubusercontent.com/12282915/138685751-98112dfd-8ed8-4185-9de7-c2e2e8f005a9.jpg)
## how does it work
The APS system works with a separate zigbee implementation. The ESP-ECU sends zigbee requests to the inverters and analyzes the answers, extracting the values. 
The ESP communicates with the zigbee module through the alternative serial port.
The ESP-ECU starts a coordinator (an entity that can start a zigbee network). The coordinator binds the inverters and sends the poll requests to them.
The interesting values are send via mqtt and displayed on the main page.

![frontpage](https://user-images.githubusercontent.com/12282915/138686152-3b065a7c-88f8-49d6-bf89-c512f82fd562.jpg)
![graph2](https://user-images.githubusercontent.com/12282915/139062602-71e92216-9703-4fc4-acc6-fabf544c4ffd.jpg)

## know issues
The zigbee module crashes sometimes during the pollings. The system recovers that by resetting it and start the coordinator again. This is done by a healthcheck that runs every 10 minutes. You can only notice this via the log and infopage where this is counted.
