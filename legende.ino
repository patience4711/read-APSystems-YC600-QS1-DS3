// nodemcu pins
//  16  D0  
//   5  D1  
//   4  D2  
//   0  D3
//   2  D4   
//   GND
//   3,3v
//   14  D5  
//   12  D6  
//   13  D7 -> P02
//   15  D8 -> P03
//   3   D9
//   1   D10

// clock was not running good, added pwm for the pulsstrength and a pulselength var

// The coordinator is the only device that can start a network
// connect 2530ZB module: rx (D7) = P03, tx (D8) = P02
// Serial of the APS inverter
// we use Serial.swap to go from normal usb serial to serial on d7, d6
// serial of my inverter 408000158215
// made the homepage to include the scripts so it uses less memory
// the inverterpage is served form progmem with processor for the form in 3 parts
// started making some global string vars in to chars freq heath acv dcc dcv power
// added an inverterquery get.Inverter?inv=x
// changed the reading procedure,
// added mqtt check to healthcheck and removed it from loop
// added a hardware reset command to use via serial
// in the healthcheck, if the zigbee not responsive it will be reset.


/*
from the readme from kadszol
Configuration ID: 0x0003; Size: 1 byte; Default value: 0

35: Sent=FE03 26 05 03 01 03 21   ZB_WRITE_CONFIGURATION  ZCD_NV_STARTUP_OPTION value=03
35: Received=FE01 66 05 00 62      SRSP

40: Sent=FE01 410000 40          RESET
40: Received=FE064180020202020702C2

45: Sent=FE0A 2605 01 08 FFFF80971B01A3D8  56 ZB_WRITE_CONFIGURATION
45: Received=FE01 66 05 00 62       SRSP status = 00 success

50: Sent=FE03 2605 87 01 00 A6      ZB_WRITE_CONFIGURATION  ZCD_NV_LOGICAL_TYPE 00=coordinator
50: Received=FE01 66 05 00 62       SRSP status = 00

55: Sent=FE04 2605 83 02 D8 A3 DD    ZB_WRITE_CONFIGURATION ZCD_NV_PANID 0x0083 id 0x02 size D8A3 identifies the network
55: Received=FE 01 66 05 00 62      SRSP status = 00

60: Sent=FE06 2605 84 04 00000100 A4  ZB_WRITE_CONFIGURATION ZCD_NV_CHANLIST on which channel it operates
60: Received=FE 01 66 05 00 62      SRSP status = 00

65: Sent=FE0D 240014050F0001010002000015000020
65: Received=FE 01 64 00 00 65
*/
