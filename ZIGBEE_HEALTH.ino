// *************************************************************************
//                   system healtcheck 
//**************************************************************************

void healthCheck() {
if(Mqtt_Format != 0) {
String Mqtt_send = Mqtt_outTopic;
if(Mqtt_outTopic.endsWith("/")) {
  Mqtt_send += "heap";
}
// we send an mqtt message about the memory

// the json to domoticz must be something like {"idx" : 7, "nvalue" : 0,"svalue" : "90;2975.00"}
       String sValue="\"svalue\":\"";   
       //sValue += String(ESP.getFreeHeap()) + ";" + String(ESP.getFreeContStack()) + "\"}"; //total of the 2 or 4
       sValue += String(ESP.getFreeHeap()) + ";" + String( stack_size() ) + "\"}"; //total of the 2 or 4
       String toMQTT = "{\"idx\":" + String(domIdx) + ",\"nvalue\":0," + sValue;
       ws.textAll("we publish heap to domoticz, mess is : " + toMQTT );
       MQTT_Client.publish ( Mqtt_send.c_str(), toMQTT.c_str(), false);  
}
//Update_Log("zigbee", "healthcheck");
   if(!timeRetrieved) getTijd();

    // reset the errorcode so that polling errors remain
    if(errorCode >= 3000) errorCode -= 3000;
    if(errorCode >= 200) errorCode -= 200;
    if(errorCode >= 100) errorCode -= 100;
    
    ZigbeePing();

    switch(checkCoordinator() ) // send the 2700 command 
       {
        case 0:
              zigbeeUp = 1; // all oke
              break;

        case 2:
              //zigbeeUp = 0;
              //String term = "zb down, received : " + String(inMessage);
              Update_Log("zigbee", "down");
              ws.textAll("zigbee down");
              resetCounter += 1;
              resetValues(false, false); // reset all values, no mqtt
              if (coordinator(true) ) zigbeeUp = 1; else zigbeeUp = 0; // try to start the coordinator
        }      

}


// *************************************************************************
//                   test if coordinator = up
//**************************************************************************

int checkCoordinator() {
// this is basicly the 2700 command  
// the answer can mean that the coordinator is up, not yet started or no answer
// we evaluate that
    char ecu_id_reverse[13];
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
    char * tail;

    // the response = 67 00, status 1 bt, IEEEAddr 8bt, ShortAddr 2bt, DeviceType 1bt, Device State 1bt
    //  FE0E 67 00 00 FFFF 80971B01A3D8 0000 0709001
    // status = 00 means succes, IEEEAddress= FFFF80971B01A3D8, ShortAdr = 0000, devicetype=07 bits 0 to 2
    
    //Device State 09 started as zigbeecoordinator     
    
    //Serial.println("checkZigbeeRadio");
    //if(log) Update_Log("zigbee", "checking zb module");
    //check the radio, send FE00670067
    // when ok the returned string = FE0E670000FFFF + ECU_ID REVERSE + 00000709001
    // so we check if we have this
    char s_d[100]={0};
    char checkCmd[10]; // we send 2700 to the zb
    strncpy(checkCmd, "270027", 9);
    //first clean up the serial port
    //empty_serial(); 
    // now we do this 3 times
    for (int x=1; x<4; x++)
    {
      strncpy(checkCmd, "270027", 9); // sendZigbee changes checkCmd so we have to renew it
      sendZigbee(checkCmd ); // sendZigbee changes checkCmd so we have to renew it
      //if ( waitSerialAvailable() ) { readZigbee(); } else { readCounter = 0;} // when nothing available we don't read

      char * inMess = readZigbee(s_d);
      delayMicroseconds(250); 
      delay(100);
      //if(diagNose) ws.textAll("inMessage = " + String(inMess) + " rc = " + String(readCounter));
  
   // we get this : FE0E670000 FFFF80971B01A3D8 0000 07090011
   //    received : FE0E670000 FFFF80971B01A3D6 0000 0709001F when ok
  
      //check if ecu_id_reverse is in the string, then split it there + 2 bytes
      if( strstr(inMess, ecu_id_reverse) )
      {
          tail = split(inMess, ecu_id_reverse + 4);
          if( strstr(tail, "0709") ) 
            {
              if(diagNose) ws.textAll("found 0709, running oke");
              return 0;
            } 
       }     
   delay(700);
   if(diagNose) ws.textAll("retrying..");
   }
   // if we come here 3 attempts failed       
    return 2;
}


void ZigbeePing() {
    // if the pin command failed then we have to restart the coordinator
    //Update_Log("zigbee", "check serial loopback");
    // these commands already have the length 00 and checksum 20 resp 26
    char pingCmd[20]={"210120"}; // ping
    char s_d[100]={0};
    //first make serial empty
    //empty_serial();
    //send command    
    sendZigbee(pingCmd); // answer is FE02 6101 79 07 1C
    //if ( waitSerialAvailable() ) { readZigbee(); } else { readCounter = 0;} // when nothing available we don't read
    //retrieve answer
    char * inMessage = readZigbee(s_d);
    //if(diagNose) ws.textAll("inMessage = " + String(inMessage) + " rc = " + String(readCounter));
    if (strstr(inMessage, "FE026101" ) == NULL) 
    {
      if(diagNose) ws.textAll("no ping answer");
    } else {
      if(diagNose) ws.textAll("ping ok");
    }
}


// *************************************************************************
//                          hard reset the cc25xx
// *************************************************************************
void ZBhardReset() 
    {
    digitalWrite(ZB_RESET, LOW);
    delay(500);
    digitalWrite(ZB_RESET, HIGH);
    String term = "ZBmodule hard reset" ;
    Update_Log("zigbee", term);
    ws.textAll(term);
    delay(2000); //wait for the cc2530 to reboot
    }
