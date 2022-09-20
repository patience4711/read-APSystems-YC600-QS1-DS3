// *************************************************************************
//                   system healtcheck 
//**************************************************************************

// it seems that after a power cycle the coordinator only has to be restarted
// with the 2600 command
void healthCheck() {
   // check moquitto
//   if(Mqtt_Enabled) {
//      if (!MQTT_Client.connected())  mqttConnect();
//   }
   // check if we need to get time
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
//        case 1:
//              zigbeeUp = 0;
//              Update_Log("zigbee", "Stopped reason: Tostart");
//              // we send the start command
//              if (coordinatorStart() ) zigbeeUp = 1;
//              break;
        case 2:
              zigbeeUp = 0;
              String term = "zb down, received : " + String(inMessage);
              Update_Log("zigbee", term);
              ws.textAll(term);
              resetCounter += 1;
              resetValues(false, false); // reset all values, no mqtt
              if (coordinator(true) ) zigbeeUp = 1; // start the coordinator
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

    char checkCommand[10]; // we send 2700 to the zb
    strncpy(checkCommand, "00270027", 9);

    // now we do this 3 times
    for (int x=1; x<4; x++)
    {
      #ifdef DEBUG
      Serial.println("swap to zb");
      swap_to_zb();
      #endif    
      
      sendZigbee(checkCommand);
      if ( waitSerialAvailable() ) { readZigbee(); } else { readCounter = 0;} // when nothing available we don't read
      if(diagNose) ws.textAll("inMessage = " + String(inMessage) + " rc = " + String(readCounter));
      #ifdef DEBUG
      Serial.println("swap to usb");
      swap_to_usb();
      Serial.println("hc received " + String(inMessage));
      #endif
  
   // we get this : FE0E670000 FFFF80971B01A3D8 0000 07090011
  //    received : FE0E670000FFFF80971B01A3D600000709001F when ok
  
      //check if ecu_id_reverse is in the string, then split it there + 2 bytes
      if( strstr(inMessage, ecu_id_reverse) )
      {
          //ws.textAll("found ecu id");
          tail = split(inMessage, ecu_id_reverse + 4);
          //ws.textAll("tail=" + String(tail)); 
          //Serial.println("\nhealth received : " + String(inMessage) );
          if( strstr(tail, "0709") ) 
            {
              if(diagNose) ws.textAll("found 0709, running oke");
              //String term = "zb up, attempts = " + String(x);
              return 0;            
            } 
//            else
//            {
//              // we know that the message contains ece_id_reverse but not 0709
//              // so the coordinator exists but is not running
//              return 1;
//            }
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
    char pingCmd[20]={"00210120"}; // ping
     
    #ifdef DEBUG
    Serial.println("send reset/start";
    swap_to_zb();
    #endif
    sendZigbee(pingCmd); // answer is FE02 6101 79 07 1C
    if ( waitSerialAvailable() ) { readZigbee(); } else { readCounter = 0;} // when nothing available we don't read
    if(diagNose) ws.textAll("inMessage = " + String(inMessage) + " rc = " + String(readCounter));
    if (strstr(inMessage, "FE026101" ) == NULL) 
    {
      if(diagNose) ws.textAll("no ping answer");
      //Update_Log("zigbee", "stopped reason no ping answer");
      
      // the next command = commented out for test
      //sendZigbee(startCmd); // start when no ping
      //if ( waitSerialAvailable() ) { readZigbee(); } else { readCounter = 0;}
      //delay(5000); //to give the zigbee system the chance to start  
      
    } else {
      if(diagNose) ws.textAll("ping ok");
    }
    
    #ifdef DEBUG
    swap_to_usb();
    #endif
    // we ignore the answer
}

//bool coordinatorStart() {
//    // send the 2600 start command
//    // this command already have the length 00 and checksum 20 resp 26
//    char startCmd[20]={"00260026"}; // zigbee start
//     
//    #ifdef DEBUG
//    Serial.println("send start cmd";
//    swap_to_zb();
//    #endif
//    
//    sendZigbee(startCmd); // answer is FE02 6101 79 07 1C
//    
//    if ( waitSerialAvailable() ) { readZigbee(); } else { readCounter = 0;} // when nothing available we don't read
//    if(diagNose) ws.textAll("inMessage = " + String(inMessage) + " rc = " + String(readCounter));
//    
//    #ifdef DEBUG
//    swap_to_usb();
//    #endif
//    // answer is FE00660066 FE0145C0098D
//    if (strstr(inMessage, "45C0" ) == NULL) 
//    {
//        if(diagNose) ws.textAll("no start answer");
//        Update_Log("zigbee", "stopped reason no start answer");
//        return false;
//     } else {
//        if(diagNose) ws.textAll("zb start success");
//        return true;
//    }
//}

// *************************************************************************
//                          hard reset the cc2530
// *************************************************************************
void hardReset() 
    {
    digitalWrite(ZB_RESET, LOW);
    delay(500);
    digitalWrite(ZB_RESET, HIGH);
    String term = "zigbee module reset" ;
    Update_Log("zigbee", term);
    ws.textAll(term);
    delay(2000); //wait for the cc2530 to reboot
    }
