# read APS inverters
This project is for reading APS Systems inverters. The program can pair and poll YC600 QS1 and DS3 inverters, up to 9 pieces. The read values are displayed on a web page and sent via mosquitto in a Json format.
The program has a lot of smart features. All settings can be done via the webinterface. It has a console that can be used for debugging and sending/receiving zigbee messages.<br>
See the <a href='https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/wiki'>WIKI</a> for information on building it, the working, etc. 

See it in action on [YouTube](https://youtu.be/WKFVQ6d8KhQ)

This program runs on different platforms, there is [ESP32-ECU](https://github.com/patience4711/ESP32-read-APS-inverters) and [RPI-ECU](https://github.com/patience4711/RPI-APS-inverters) that runs on a Raspberry (Zero). Each have its advantages over the others. If you have many inverters you are probably better off with the raspberry implementation.

## compile this sketch
You can use the provided binary but if you must compile it yourself: Use arduino ide with the esp822 version 2.7.1 installed under boardmanager. The ota updates won't work with other versons.
<br>Please note: by popular demand I have published the code here but i do not feel responsible for problems as to compiling. Impossible for me to know why it doesn't compile in your situation.

## downloads
april 5 2023: There is a new version v9_8 available, See changelog.<br>
Download [ESP-ECU-v9_8](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/blob/main/ESP-ECU_v9_8.bin)<br>

april 1 2023: There is a new experimental version [ESP-ECU_v9_7_beta](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/blob/main/ESP-ECU_v9_7_beta.bin) available. This version has output of all inverters together in the frontpage. Look [here]( https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/issues/89) for more info.

march 24 2023: 18:00hr There is a recompiled version v9_6c available, See changelog.<br>
Download [ESP-ECU-v9_6c](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/blob/main/ESP-ECU_v9_6c.bin)<br>

<br>In case someone wants to print the housing, here is an [stl file](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/blob/main/ESP-ECU-housing.zip)
This is for a nodemcu board 31x58mm.

## features
- Simply to connect to your wifi
- Easy add, delete and pair inverters
- automatic polling or on demand via mqtt or http
- data can be requested via http and mosquitto
- There are 4 different mqtt json formats
- Fast asyc webserver
- a very smart on-line console to send commands and debugging
- Smart timekeeping
- A lot of system info on the webpage

## the hardware
It is nothing more than an esp device like nodemcu, wemos or its relatives and a prepared cc2530, cc2531 zigbee module. And a powersupply.
The zigbeemodule should be flashed with a firmware that is developped by kadsol : [CC25xx_firmware](https://github.com/Koenkk/zigbee2mqtt/files/10193677/discord-09-12-2022.zip). The firmware is also available [here](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/blob/main/cc25xx_firmware.zip). Much more info as to the development of this software can be found here https://github.com/Koenkk/zigbee2mqtt/issues/4221. 

For info on how to build and use it, please see the <a href='https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/wiki'>WIKI</a>

![minhw](https://user-images.githubusercontent.com/12282915/138685751-98112dfd-8ed8-4185-9de7-c2e2e8f005a9.jpg)
## how does it work
APS works with their own zigbee implementation. The ESP-ECU sends zigbee commands (wireless) to the inverters and analyzes the answers, extracting the values. 
The ESP communicates with the zigbee module through the alternative serial port (wired).
The ESP-ECU starts a coordinator (an entity that can start a zigbee network). The coordinator binds the inverters and sends the poll requests to them.
The interesting values are send via mqtt and displayed on the main page.

![frontpage](https://user-images.githubusercontent.com/12282915/138686152-3b065a7c-88f8-49d6-bf89-c512f82fd562.jpg)
![graph2](https://user-images.githubusercontent.com/12282915/139062602-71e92216-9703-4fc4-acc6-fabf544c4ffd.jpg)

## changelog ##
version ESP-ECU_V9_8:
- fix for the zigbee crashes
- improved frontpage with buttons to see details.

version ESP-ECU_V9_6c:
- fixed a typo in the help page of mosquitto settings
- added 2 mqtt json formats, numeric values are included as float now.
- added the retain flag to mqtt formats 3 and 4
- changed the format of the returned json when http request.
- added an option to have an mqtt topic for each inverter.
- some changes to improve the polling process
- added a forced zigbee reset every midnight.

version ESP-ECU_V9_5:
- added a mqtt command for polling all inverters
- added a signal quality value

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
