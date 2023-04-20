void polling(int which) {
    polled[which]=false; //nothing is displayed on webpage
    if(zigbeeUp == 0) 
    {
      if(diagNose) ws.textAll("skipping poll, coordinator down!"); //
      return;
    }
    if(diagNose) ws.textAll("polling"); //
    char pollCommand[67] = {0};
    char ecu_id_reverse[13];
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);    


//    if(diagNose) ws.textAll("zb send poll cmd inverter " + String(which) + "  cmd:" + String(pollCommand));   
      snprintf(pollCommand, sizeof(pollCommand), "1D2401%.2s%.2s1414060001000F13%sFBFB06BB000000000000C1FEFE", Inv_Prop[which].invID + 4, Inv_Prop[which].invID + 2, ecu_id_reverse);
     //add checksum
     //strcpy(pollCommand, strncat(pollCommand, checkSum(pollCommand), sizeof(pollCommand) + sizeof(checkSum(pollCommand))));
     //APPEND_CRC(initCmd);
     //strncat(pollCommand, checkSum(pollCommand), 2);
     //delayMicroseconds(250);
        
//        #ifdef DEBUG
//        swap_to_zb(); // set serial to zb  
//        #endif

    if(diagNose) ws.textAll("zb send poll cmd inverter " + String(which) + "  cmd:" + String(pollCommand));
    sendZigbee(pollCommand);
     //delay(1000); now we wait max 2 seconds
    if ( waitSerialAvailable() ) { 
      if(actionFlag == 44) actionFlag=0; // reset the need for healthcheck
      readZigbee(); 
      } else { 
        readCounter = 0;
        actionFlag = 44; // need for healthcheck
        } // when nothing available we don't read

    //  readSerial(); test
 
      if(diagNose) ws.textAll("polling answer " + String(inMessage));

      //now we have a global char inMessage like 
      //F8FE0164010064 answer to poll request status 6401 00=success
      //FE03 4480 001401D2 answer after datarequest 00 = status succes 14=endpoind 01=transactionid
      //FE03 45C4 3A10 00A8 inform host device the receipt of a source route to a given device 3A10 = dest addr
      //FE724481000006013A10141400240042B4C500005E408000158215FBFB51B104280F405B00008CDF00000001F00262906604990000000000200001B716676600004429E11F00030555073F0303030100000100000000000002000000000000000000000000000000000000000000000000FEFE3A100EBC

      //we are going to analyze this
      //if message is empty or not a correct answer, this is decided in decodePollAnswer 
      
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

bool waitSerialAvailable() // wait untill something 's available
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


  
