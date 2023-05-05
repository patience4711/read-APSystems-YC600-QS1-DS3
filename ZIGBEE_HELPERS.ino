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
    //add crc
    strcat(bufferSend, checkSum(bufferSend));
    delayMicroseconds(250);
    
    //clean up serial buffer
    empty_serial();

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
}

// *****************************************************************************
//                 read zigbee radio 
// *****************************************************************************
// this function returns the incoming message as a char*
char * readZB( char inMessage[] ) {
    readCounter = 0;
    //wait a while untill something available
    //if not within the reactiontime we return NULL
    if( !waitSerialAvailable() ) {
        inMessage[0]='\0';
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
        return inMessage;
}
//// *****************************************************************************
////                 send to zigbee radio
//// *****************************************************************************
//void sendZigbee( char printString[] )
//{
//    char bufferSend[3];
//    // first add sLen and crc
//    strcpy(printString, strcat(sLen(printString), printString));
//    delayMicroseconds(250);
//    strcat(printString, checkSum(printString));
//    delayMicroseconds(250);
//
//    if (Serial.availableForWrite() > (uint8_t)strlen(printString))
//    {
//        Serial.write(0xFE); //we have to send "FE" at start of each command
//        for (uint8_t i = 0; i <= strlen(printString) / 2 - 1; i++)
//        {
//         // we use 2 characters to make a byte
//            strncpy(bufferSend, printString + i * 2, 2); 
//            delayMicroseconds(250);                     //give memset a little bit of time to empty all the buffers
//            Serial.write(StrToHex(bufferSend));        //turn the two chars to a byte and send this
//        }
//            Serial.flush(); //wait till the full command was sent
//    }
//    if (diagNose) ws.textAll("sendZigbee sent FE" + String(printString));
//}

//// *****************************************************************************
////                 read zigbee radio at swapped serial
//// *****************************************************************************
//// this function is somewhat unefficient but that seems an advantage due to the 
//// slow reaction from the inverter
//void readZigbee() {
//    readCounter = 0;
//    char oneChar[3] = {0};
//    //fullIncomingMessage[0] = '\0'; //terminate otherwise all is appended
////    memset( &inMessage, 0, sizeof(inMessage) ); //zero out the 
////    delayMicroseconds(250);
//        
//    while (Serial.available())
//        {
//        // here we have the danger that when readcounter reaches 512, there are 1024 bytes processed 
//        // the length of a poll answer is usually not more than 223
//        if (readCounter < CC2530_MAX_SERIAL_BUFFER_SIZE/2)
//            {
//                sprintf(oneChar, "%02X", Serial.read()); // always uppercase
//                strncat(inMessage, oneChar, 2); // append 
//                //processIncomingByte(Serial.read());
//                readCounter += 1;
//            }
//            else
//            {
//                //Serial.read(); // we read from serial to empty the buffer but do not process
//                ESP.wdtFeed();
//                empty_serial(); // remove all excess data in the buffer at once
//            }
//            if (Serial.available() == 0) delay(120); // we wait if there comes more data
//        }
//            //if we come here there is no serial data anymore
// 
//    cleanIncoming(); // check for F8 and remove it
//    if (diagNose) ws.textAll("readZigbee " + String(inMessage));
//}

//// format the incoming byte and add it to inMessage
//void processIncomingByte(const byte inByte)
//{
//char oneChar[10] = {0};
//    sprintf(oneChar, "%02x", inByte);
//    strncat(inMessage, oneChar, 2); // append 
//} // end of processIncomingByte


//// this is done in readZB
//void cleanIncoming() {
//// with swaps we get F8 sometimes, this removes it
//   if(inMessage[0] == 'F' && inMessage[1] == '8') {
//       strcpy(inMessage, &inMessage[2]);
//   }
//}

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
void inverterReset(int which) {
      char ecu_id_reverse[13];  
      ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
      char inv_id[7];
      strncpy(inv_id, Inv_Prop[which].invID, strlen(Inv_Prop[which].invID));
      char resetCmd[80];
      
      char command[][50] = {
        "2401",
        "1414060001000F13",
        "FBFB06C1000000000000A6FEFE",
        };
      strncpy( resetCmd, command[0], sizeof(command[0]) );
      strncat( resetCmd, inv_id + 4, 2 ); // ad the 2nd byte of inv_id
      strncat( resetCmd, inv_id + 2, 2 );     // ad the 1st byte of inv_id  
      strncat( resetCmd, command[1], sizeof(command[1]) );
      strncat( resetCmd, ecu_id_reverse, sizeof(ecu_id_reverse) );
      strncat( resetCmd, command[2], sizeof(command[2]) );
      ws.textAll("the resetCmd = " + String(resetCmd));

      //should be 2401 103A 1414060001000F13 80 97 1B 01 A3 D6 FBFB06C1000000000000A6FEFE
      // got      1414060001000F1380971B01A3D6FBFB06C1000000000000A6FEFE


      //
      //strcpy(resetCmd, strcat(sLen(resetCmd), resetCmd));
      // put sln at the beginning and CRC at the end done by sendZigbee()
    
      sendZB(resetCmd);
      delay(1000);
      char s_d[150]={0};
      char * inMess = readZB(s_d);

      if(readCounter == 0) {
      } 
      ws.textAll("received : " + String(inMess) );
}

// ******************************************************************************
//                    reset all values and send mqtt
// ******************************************************************************
void resetValues(bool energy, bool mustSend) {
      for(int z=0; z<inverterCount; z++) 
      { 
         for(int y=0; y<5; y++ ) strcpy(Inv_Data[z].power[y], "0.0"); // make powervalues null
         ws.textAll("reset power values");
         if(energy) 
         {
            Inv_Data[z].en_total = 0; // make energy null
            ws.textAll("reset energy values");
         }
         if(mustSend) mqttPoll(z); // send the null values
      }
      
}
