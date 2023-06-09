void polling(int which) {
      polled[which]=false; //nothing is displayed on webpage
      if(zigbeeUp == 0) 
      {
        if(diagNose) ws.textAll("skipping poll, coordinator down!"); //
        return;
      }
      if(diagNose) ws.textAll("polling"); //
      char pollCmd[67] = {0};
      char ecu_id_reverse[13];
      ECU_REVERSE().toCharArray(ecu_id_reverse, 13);    
   
      snprintf(pollCmd, sizeof(pollCmd), "2401%s1414060001000F13%sFBFB06BB000000000000C1FEFE", Inv_Prop[which].invID, ecu_id_reverse);
      //add checksum done in sendZigbee

      delayMicroseconds(250);
       
      if(diagNose) ws.textAll("zb send poll cmd inverter " + String(which));
    
      sendZB(pollCmd);

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
                  if(diagNose) ws.textAll("polling failed " + String(errorCode));
        }
}

// ****************************************************************************************
//          wait a certain time to see if there is something avalable
// ****************************************************************************************
bool waitSerialAvailable() // wait untill something 's available
{
  unsigned long wait = millis();
  while ( !Serial.available() )
      {
      if ( millis() - wait > 2000) return false; // return false after 2000 milis time out
      }
  // if we are here there is something available
   delay(500);
   return true;
}


  
