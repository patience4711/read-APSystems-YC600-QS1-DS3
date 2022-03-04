void polling(int which) {
    polled[which]=false; //nothing is displayed on webpage
    if(zigbeeUp == 0) 
    {
      if(diagNose) ws.textAll("skipping poll, coordinator down!"); //
      return;
    }
    char pollCommand[254] = {0};
    char normalOperationBaseCommand[][254] =
    {
        "2401",                      //+ ..string.sub(inv_id[x],3,4)..string.sub(inv_id[x],1,2)..
        "1414060001000F13",          //append (concatenate) _ecu_id_reverse
        "FBFB06BB000000000000C1FEFE" // end of String
    };

    //Serial.println("Inv_Prop[which].invID = " + String(Inv_Prop[which].invID) );
    char inv_id[7];
    memset(&inv_id[0], 0, sizeof(inv_id)); //zero out otherwise we get strange tokens
    delayMicroseconds(250);
    
    strncpy(inv_id, Inv_Prop[which].invID, strlen(Inv_Prop[which].invID));
    inv_id[7]='\0'; //terminate

    char ecu_id_reverse[13];  
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
    
    strncpy(pollCommand, normalOperationBaseCommand[0], strlen(normalOperationBaseCommand[0])); // the 2401
    //Serial.println("pollCommand now = " + String(pollCommand));
    strncat(pollCommand, inv_id + 4, 2 ); // ad the 2nd byte of inv_id
    strncat(pollCommand, inv_id + 2, 2 );     // ad the 1st byte of inv_id
    //Serial.println("pollCommand after reversed inv id = " + String(pollCommand));
    strncat(pollCommand, normalOperationBaseCommand[1], sizeof(normalOperationBaseCommand[1]));
    //Serial.println("pollCommand after 1414060001000F13 = " + String(pollCommand));         
    strncat(pollCommand, ecu_id_reverse, sizeof(ecu_id_reverse));
    //Serial.println("pollCommand after reversed in id = " + String(pollCommand));
    strncat(pollCommand, normalOperationBaseCommand[2], sizeof(normalOperationBaseCommand[2]));
    //Serial.println("pollCommand after FBFB06BB000000000000C1FEFE = " + String(pollCommand));
    
    // calculate length and put this at beginning
    strcpy(pollCommand, strncat(sLen(pollCommand), pollCommand, sizeof(sLen(pollCommand)) + sizeof(pollCommand))); //build command plus sln at the beginning
    // put in the CRC at the end of the command
    strcpy(pollCommand, strncat(pollCommand, checkSum(pollCommand), sizeof(pollCommand) + sizeof(checkSum(pollCommand))));
    //DebugPrintln("pollCommand:");  //DebugPrintln(String(pollCommand));
    if(diagNose) ws.textAll("zb send poll cmd inverter " + String(which));   
        #ifdef DEBUG
        swap_to_zb(); // set serial to zb  
        #endif

    sendZigbee(pollCommand);
     //delay(1000); now we wait max 2 seconds
    if ( waitSerialAvailable() ) { readZigbee(); } else { readCounter = 0;} // when nothing available we don't read

        #ifdef DEBUG
        swap_to_usb(); // set serial to zb  
        #endif

 
      if(diagNose) ws.textAll("polling answer " + String(inMessage));

      //now we have a global char inMessage like 
      //F8FE0164010064 answer to poll request status 6401 00=success
      //FE03 4480 001401D2 answer after datarequest 00 = status succes 14=endpoind 01=transactionid
      //FE03 45C4 3A10 00A8 inform host device the receipt of a source route to a given device 3A10 = dest addr
      //FE724481000006013A10141400240042B4C500005E408000158215FBFB51B104280F405B00008CDF00000001F00262906604990000000000200001B716676600004429E11F00030555073F0303030100000100000000000002000000000000000000000000000000000000000000000000FEFE3A100EBC

      //we are going to analyze this
      //if message is emty or not a correct answer, this is established in decodePollAnswer 
      
      errorCode = decodePollAnswer(which);     
      switch( errorCode )
      {

          case 0:
                   polled[which] = true;
                   yield();
                   mqttPoll(which); //
                   yield();
                   break;
          default:
                ws.textAll("polling failed " + String(errorCode));
      }
}



bool waitSerialAvailable() 
{
  //We start with a minimum delay
  //delay(800);
  unsigned long wait = millis();
  while ( !Serial.available() )
      {
      if ( millis() - wait > 2000) return false; // return false after 2000 milis time out
      }
  // if we are here there is something available
   delay(500);
   return true;
}


  
