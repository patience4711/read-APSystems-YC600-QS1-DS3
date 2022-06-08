# read APS inverters
This project is for reading APS Systems inverters. The program can pair and poll YC600 QS1 and DS3 inverters, up to 9 pieces. The read values are displayed on a web page and sent via mosquitto in a Json format.
The program has a lot of smart features. All settings can be done via the webinterface. It has a console that can be used for debugging and sending/receiving zigbee messages.<br>
See the <a href='https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/wiki'>WIKI</a> for information on building it, the working, etc. 

See it in action on youtube: https://youtu.be/7ZOAcrYXxbM

## downloads
may 17 2022: There is a new version v9_4 available, See changelog.<br>
Download ESP-ECU-v9_4 https://1drv.ms/u/s!AkMG5FvUwhedi0bA08s8itWhnSJA?e=risayF <br>

<br><br>In case someone wants to print the housing, here is an stl: https://1drv.ms/u/s!AkMG5FvUwhedim8a_tVLywAivVDI?e=XIwhYg
This is for a nodemcu board 31x58mmr>

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

Or use the binary file ESP-ECU-v9_4 to flash directly on your ESP device.

## the hardware
It is nothing more than an esp device like nodemcu, wemos or its relatives and a prepared cc2530, cc2531 zigbee module. And a powersupply.
The zigbeemodule should be flashed with a firmware that can be found here https://github.com/Koenkk/zigbee2mqtt/issues/4221. Here you can find much more information on this project.
The most recent link to a lua implementation (including firmware) by kadszol :https://github.com/Koenkk/zigbee2mqtt/files/6797510/discord-11-7-2021.zip

the wiring: ( also see the wiki and issues )
### cc2530/31 -> ESP 
-  p02   -> d8
-  p03   -> d7
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

## changelog ##
version ESP-ECU_V9_4:
- fixed failed polling after power cycle
- fine-tuned some dc values of the DS3
- default conversion factor now 1.66
 
version ESP-ECU_V9_3:
- fixed a bug with a negative poll offset
- added the poll- begin and end time on the main page

version ESP-ECU_V9_2:
- made it suitable for the DS3 inverter.
- fixed polling intervall of 5 minutes.
- bug repaired, sq1 dc data panels 2 and 3

As of version 8 the value of total energy is no longer directly comming from the inverter's register. Instead all energy increases are added up in a total energy value. 
This way that value is not sensitive to inverter resets that will happen in twilight or due to an eclips etc.
