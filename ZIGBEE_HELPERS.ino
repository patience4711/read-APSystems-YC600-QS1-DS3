// *****************************************************************************
//                 send to zigbee radio
// *****************************************************************************
void sendZB( char printString[] )
{

    char bufferSend[254]={0};
    char byteSend[3]={0};
    // add sLen 
    sprintf( bufferSend, "%02X", (strlen(printString) / 2 - 2) );
    strcat(bufferSend, printString);
    delayMicroseconds(250); //
    //Serial.println("bufferSend sLen = " + String(bufferSend));
    //add crc
    strcat(bufferSend, checkSum(bufferSend));
    delayMicroseconds(250);
    
    //Serial.println("bufferSend crc = " + String(bufferSend));
    //swap_to_zb();//clean up serial buffer
    //empty_serial();

    if (Serial.availableForWrite() > (uint8_t)strlen(printString))
    {
        Serial.write(0xFE); //we have to send "FE" at start of each command
        for (uint8_t i = 0; i <= strlen(bufferSend) / 2 - 1; i++)
        {
         // we use 2 characters to make a byte
            strncpy(byteSend, bufferSend + i * 2, 2); 
            delayMicroseconds(250);                     //give memset a little bit of time to empty all the buffers
            Serial.write(StrToHex(byteSend));        //turn the two chars to a byte and send this
        }
            Serial.flush(); //wait till the full command was sent
    if (diagNose) ws.textAll("sendSB sent FE" + String(bufferSend));
    }
//swap_to_usb();
    //Serial.println("sent " + String(bufferSend));
}

// *****************************************************************************
//                 read zigbee radio 
// *****************************************************************************
// this function returns the incoming message as a char*
char * readZB( char inMessage[] ) {

//swap_to_zb();  
    readCounter = 0;
    //wait a while untill something available
    //if not within the reactiontime we return NULL
    if( !waitSerialAvailable() ) {
        inMessage[0]='\0';
        if(diagNose) ws.textAll("readZB nothing received");
        return inMessage;
    }

    char oneChar[10] = {0};
        
    while (Serial.available())
        {
        
        if (readCounter < CC2530_MAX_SERIAL_BUFFER_SIZE/2)
            {
                sprintf(oneChar, "%02X", Serial.read()); //always uppercase
                strncat(inMessage, oneChar, 2); // append 
                //processIncomingByte not needed anymore
                readCounter += 1;
            }
            else
            {
                ESP.wdtFeed(); // prevent timeout
                empty_serial(); // remove all excess data in the buffer at once
            }
            if (Serial.available() == 0) delay(120); // we wait if there comes more data
        }
        
        // now we should have catched inMessage
        if(readCounter == 0) inMessage[0]='\0';
        // with swaps we get F8 sometimes, this removes it
        if(inMessage[0] == 'F' && inMessage[1] == '8') {
           strcpy(inMessage, &inMessage[2]);
        }

        if(diagNose) {
          delayMicroseconds(250); // give it some time 
          ws.textAll("readZB " + String(inMessage) + "  rc=" + String(readCounter) );
          delayMicroseconds(250); // give it some time 
        }
//swap_to_usb();
        return inMessage;
}



// **************************************************************************
//                               data converters
// **************************************************************************

// calculate and return the length of the message
char *sLen(char Command[])  
{
char bufferSln[254];
    sprintf(bufferSln, "%02X", (strlen(Command) / 2 - 2));
    delayMicroseconds(250); //give memset a little bit of time to empty all the buffers

    return bufferSln;
}

// calculate and return the checksum of the message ****************************
char *checkSum(char Command[])
{
char bufferCRC[254] = {0};
char bufferCRCdiezweite[254] = {0};

    strncpy(bufferCRC, Command, 2); //as starting point perhaps called "seed" use the first two chars from "Command"
    delayMicroseconds(250);         //give memset a little bit of time to empty all the buffers

    for (uint8_t i = 1; i <= (strlen(Command) / 2 - 1); i++)
    {
        strncpy(bufferCRCdiezweite, Command + i * 2, 2); //use every iteration the next two chars starting with char 2+3
        delayMicroseconds(250);                          //give memset a little bit of time to empty all the buffers
        sprintf(bufferCRC, "%02X", StrToHex(bufferCRC) ^ StrToHex(bufferCRCdiezweite));
        delayMicroseconds(250); //give memset a little bit of time to empty all the buffers
    }

    return bufferCRC;
}

// **************************  convert a char to Hex ******************************************************
int StrToHex(char str[])
{
    return (int)strtol(str, 0, 16);
}

// *************************  reverse the ecu id **********************************************************
String ECU_REVERSE() {
String ecu_id = String(ECU_ID);
String reverse = ecu_id.substring(10,12) + ecu_id.substring(8,10) + ecu_id.substring(6,8) + ecu_id.substring(4,6) + ecu_id.substring(2,4) + ecu_id.substring(0,2);
return reverse;
}

// ******************************************************************************
//                   reboot an inverter
// *******************************************************************************
void inverterReboot(int which) {
    char ecu_id_reverse[13];  
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
    if(zigbeeUp != 1) {
       ws.textAll("skip inverter reboot, zigbee down");
       return; 
    }

//swap_to_usb ();
//    Serial.println("sending the reboot message");
    
    //char inv_id[7];
    //strncpy(inv_id, Inv_Prop[which].invID, strlen(Inv_Prop[which].invID));
    char rebootCmd[57]={0};
    char s_d[200]={0};
    
      char command[][50] = {
        "2401",
        "1414060001000F13",
        "FBFB06C1000000000000A6FEFE",
        };
//Serial.println("constructing command");
      strncpy( rebootCmd, command[0], sizeof(command[0]) );
      strncat( rebootCmd, Inv_Prop[which].invID, 4 ); // ad the 2nd byte of inv_id
      strncat( rebootCmd, command[1], sizeof(command[1]) );
      strncat( rebootCmd, ecu_id_reverse, sizeof(ecu_id_reverse) );
      strncat( rebootCmd, command[2], sizeof(command[2]) );
//Serial.println("command: " + String(rebootCmd));
      ws.textAll("the rebootCmd = " + String(rebootCmd));

       //2401 BBAA 1414060001000F13 80971B01A3D6 FBFB06C1000000000000A6FEFE
      //should be 2401 103A 1414060001000F13 80 97 1B 01 A3 D6 FBFB06C1000000000000A6FEFE
      // got      1414060001000F1380971B01A3D6FBFB06C1000000000000A6FEFE
      //          2401 BBAA 1414060001000F13 80971B01A3D6 1414060001000F13
      //  2401 3A10 1414060001000F13 80971B01B3D6 FBFB06C1000000000000A6FEFE
      // put sln at the beginning and CRC at the end done by sendZigbee()
//swap_to_zb();    

      sendZB(rebootCmd);
      delay(1000);
      //char s_d[150]={0};
      readZB(s_d);

//      if(readCounter == 0) {
//      } 
//      ws.textAll("received : " + String(inMess) );
}

// ******************************************************************************
//                    reset all values and send mqtt
// ******************************************************************************
void resetValues(bool energy, bool mustSend) {
      for(int z=0; z<inverterCount; z++) 
      { 
         for(int y=0; y<5; y++ ) Inv_Data[z].power[y] = 0.0; // make powervalues null for all panels
         if(diagNose) ws.textAll("reset power values inv " + String(z));
         if(energy) 
         {
            Inv_Data[z].en_total = 0; // make energy null
            if(diagNose) ws.textAll("reset energy values inv " + String(z));
         }
         if(mustSend) mqttPoll(z); // send the null values
      }
 }

 void swap_to_usb () {
  delay(1000);
  if (!USB_serial)
  {
      Serial.swap();
      delay(300);
      empty_serial(); // remove what's in the buffer
      USB_serial=true;
  } 
}

void swap_to_zb () {
   delay(200); //was 300
  if (USB_serial) 
  {
      Serial.swap();
      delay(100);
      empty_serial(); // remove what's in the buffer
      USB_serial=false;
   }
}
