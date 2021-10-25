
INTRODUCTION
This program can read data from APS-Systems inverters, both YC600 and SQ1.
The data is displayed on webinterface and sent via mqtt.
You can use it on a NodeMCU, Wemos, ESP12-E may work too.
It is very easy to use, all settings are done via a webinterface.

It works with the zigbee module as on the "cc2530.jpg photo in this folder.
Other zigbee modules seem to work also but that has not been tested by me.
The minimal hardware you need, see photo minhw.jpg

This version has been created after much work, testing and exploring reasonable questions/comments from others. The system works on the edge of ESP capabilities. If you want more precission or more data, that is not possible with this hardware. 

FLASHING THE ZIGBEE MODULE
You need to flash the module with the firmware you can find in the folder cc25-- firmware.
For this you need no extra hardware as you can do it with ccloader that you install on the nodemcu.(google)
The easyest way to do this is with the help of a Raspberry pi.(google)

FLASHING THE ESP
When you unpacked this folder, you can click on the file "Serial_communicator.exe".
It provides all you need to flash and communicate with your ESP device.

If you are an experienced user, you can just click 
esp8266_flasher.exe, next:
 - boot your esp in programm mode 
 - select the ECU-SOLARxxx.bin file and a com port 
 - click download

Esp programm mode: While holding the flash button down, plug the usb cable in
or
Hold the flasbutton down and click the reset (rst) button.

UPDATE THE SOFTWARE
You only have to flash your ESP once, software updates can be installed "over the air" (OTA).
via the menu go to the fw update page. Choose the ECU-SOLAR-xxx.bin file and click update.


GET STARTED
When booted, the blue led on the nodemcu is on. This means an accesspoint has been started for entering your wifi credentials. Connect to this, browse to 192.168.4.1 and enter your data. This works best on an android device. Now your nodemcu is connected to your wifi. It will tell you its IP address, so you can browse to its webpage.

SETTINGS
First visit the time config page, these are important settings as automatic polling only works between sunrise and sunset.
In the general settings you can set the pollresolution. It has no use to poll at every 10 seconds, you only overload your by system doing that. It's an ESP, not a mainframe.
If you uncheck the polling checkbox, you can do the polling via external requests (http or mqtt) see chapter communication. 
In order to poll an inverter it first has to be paired to the zigbee network. You should know the serialnr of the inverter to pair it.

PAIRING
Go to the inverter page, fill up the form. 
The serial nr. can be found on your inverter (12 characters, yc starts with 408000)
DOM idx is an identifier that is transmitted via mqtt, the recipient can use this to determine which device the data is for.
Save the data.
Now the pairing button appears. If you click that, the pairing process is started.
Tip: If you open the console in another webpage, you can follow this process. ( type 10;diag first)


FUNCTION OF THE FLASH BUTTON AND THE LED
With the button we can reboot the Esp or start the wifi configuration modus (accesspoint)
Reboot; press the button. after a short flash of the led, the led lights up. Now release the button.
Start AP; press the button. after a short flash of the led, the led lights up, keep pressing untill led goes off. Now release the button. 
All wifisettings will be erased and the accesspoint is started.

When the wificonfigurationpoint is active, the led is on. At bootup the led flashes 3 times, this means the ESP is wifi connected.
When starting the zigbee system the led flashed at every send zigbee command. When started successfully, one can see 5 short flashes.
If the polling is done, there is one short flash.

HARDWARE
Connect the cc2530 see the diagram ESP-ECU_wiring.jpg in this folder 
cc2530 -> ESP 
  p2   -> d8
  p3   -> d7
  vcc  -> 3.3v
  gnd  -> gnd
  rst  -> d5

You can optional connect an LED and a tactile button, they have the same functionallity as the onboard flash button and blue led.


DEBUGGING VIA UART
Use the serial communicator:
connect to your COM port with baud 115200.
You can only see some boot information. After boot you can't use the serial port anymore. 


COMMUNICATION
The inverter data is sent via mosquitto in a json format.
The format depends on the send topic. Check "help" on the mosquitto page to see the formats.
You can query the current data (this command does not poll) of an inverter by sending: 
"ip_of_ecu/get.Inverter?inv=x"
You'l get a jsonstring like: 
{"serial":"408000158215","freq":"00.0","temp":"00.0","avc":"000.0","dcv0":"00.0","dcc0":"00.0","pow0":"000.0","dcv1 .... etc


The automatic polling can be disabled. This alows the user to issue a mosquitto message to the in-topic, e.g. {"poll":0}. 
We can also use an api to force polling e.g. http://ip_of_ecu/POLL=0
This way we can let our domotica programs ask the ecu for data. For security reasons the api works only from inside your network.

DIAGNOSTICS
This software has an online console that we can use to observe some processes to help us debug.
In the console type <10;diag> to toggle the diagnose info.
Or check the infopage for the current errorcode.
for more information read Z-Stack.Monitor.and.Test.API.pdf on the web.
errorcodes:
10: AF_DATA_REQUEST failed  (answer is not FE01640100)
11: no AF_DATA_CONFIRM  (answer does not contain FE03448000)
12: no  ZDO_SRC_RTG_IND (answer does not contain FE0345C4)
13: no  AF_INCOMING_MSG (answer does not contain 4181)
50: nothing received
100: no reaction from zigbee hardware (not present?)
200: coordinator not up
3000; Heap < 500
combined: 3050 means low memory and no reception of zb messages

TROUBLESHOOTING
software update:
A new software can imply that the filesystem has a new variable. Or another flash configuration.
This could mean that the existing variables are overwritten and the ESP-ECU doesn't work properly.
If you experience malfunctions (for example not polling) after an update, check all your settings and save them again.

zigbee:
If the coordinator won't start, check your wiring. Be sure the right firmware is on your module.
To test the zigbee hardware you can type "10;ZBT=2710AABBCC" in the console. You should
get inMessage: FE036710AABBCCA9, the answer contains AABBCC which proves that the serial
connection betweeen the ESP and the zigbeemodule works. The Hardware is ok.

pairing:
Depending on the state of the inverter (was it paired to another ecu before?) it can take some time
or several pair attempts before it succeeds. If you follow the init process in the console, you can see if there are answers coming in from the inverter. 
If not, maybe its too dark or too far form the inverter

polling:
If you see in the console that there are no polling answers, it's maybee too dark, sometimes this happens on a very cloudy day. Or your ESP-ECU is too far away from the inverter. 

WORKING OF THE SYSTEM
At the elaps of the poll resolution time a request for data is send to the inverter.
The inverter answers with some confirmations and the data.
The generated energy is the most important data in this regard.
With the help of the energy generated during the elapsed time, we can calculate the power,
which is an average over the elapsed time. 

FAQ
Q: Can i have more data and / or more precission.
A: No, please read this document


