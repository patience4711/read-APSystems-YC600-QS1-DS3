// *************************************************************************
//                   system healtcheck 
//**************************************************************************

void healthCheck() {
    // should we send the heap
#ifndef MEMCHECK
    memoryCheck();
#endif
//    if(Mqtt_Format != 0 && Mqtt_Format != 5 && Mqtt_stateIDX != 0) {
//       if (mqttConnect() ) {
//            char Mqtt_send[26];
//            strcpy( Mqtt_send, Mqtt_outTopic) ;
//            if( Mqtt_send[strlen(Mqtt_send)-1] == '/' ) {
//                 strcat(Mqtt_send, "heap");
//            }
//      
//        char toMQTT[50]={0}; // when i make this bigger a crash occures
//        
//        sprintf( toMQTT, "{\"idx\":%d,\"svalue\":\"%ld;%ld\"}", Mqtt_stateIDX, ESP.getFreeHeap(), ESP.getFreeContStack()); 
//        //must be like {"idx":901,"nvalue":0,"svalue":"22968;1904"}   length 44
//    
//        if(diagNose) ws.textAll("mqtt publish heap, mess is : " + String(toMQTT) );
//        MQTT_Client.publish ( Mqtt_send, toMQTT, false);  
//        
//        memset(&toMQTT, 0, sizeof(&toMQTT)); //zero out 
//        delayMicroseconds(250);
//       }  
//    }

   
    if(!timeRetrieved) { 
      if(diagNose) ws.textAll(F("need to get time"));
      getTijd();

      eventSend(1);
    }

    // reset the errorcode so that polling errors remain
    if(errorCode >= 3000) errorCode -= 3000;
    if(errorCode >= 200) errorCode -= 200;
    if(errorCode >= 100) errorCode -= 100;
    
        if( inverterCount < 1 ) {
          if (diagNose) ws.textAll(F("skipping, no inverters present"));
          #ifdef LOG 
          Update_Log( 2, "no inverter"); 
          #endif
          zigbeeUp = 0;
          return;
        }


    switch(checkCoordinator() ) // send the 2700 command 
       {
        case 0:
              zigbeeUp = 1; // all oke
              diagNose = false;
              break;

        case 2:
              // the coodinator is down so we try to start it
              if(diagNose) ws.textAll("zigbee down");
              resetCounter += 1;
              resetValues(false, false); // reset all values (exept total energy), no mqtt
              if (coordinator(true) ) zigbeeUp = 1; else zigbeeUp = 0; // try to start the coordinator
              //events.send( "reload", "message"); // we reload the frontpage to reflect the state
              eventSend(0);
        }      

}


// *************************************************************************
//                   test if coordinator = up
//**************************************************************************

int checkCoordinator() {
// this is basicaly the 2700 command  
// the answer can mean that the coordinator is up, not yet started or no answer
// we evaluate that
    char ecu_id_reverse[13];
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
    char * tail;
    char s_d[200]={0}; // to provide a buffer for readZB
    // the response = 67 00, status 1 bt, IEEEAddr 8bt, ShortAddr 2bt, DeviceType 1bt, Device State 1bt
    //  FE0E 67 00 00 FFFF 80971B01A3D8 0000 0709001
    // status = 00 means succes, IEEEAddress= FFFF80971B01A3D8, ShortAdr = 0000, devicetype=07 bits 0 to 2
    
    //check the radio, send FE00670067
    // when ok the returned string = FE0E670000FFFF + ECU_ID REVERSE + 00000709001
    // so we check if we have this

    char checkCmd[10]; // we send 2700 to the zb
    strncpy(checkCmd, "2700", 5);
    //first clean up the serial port
    empty_serial(); 

      sendZB(checkCmd );
      char inMess[254]={0};  
      // retrieve the answer
      strcpy(inMess, readZB(s_d));
  
      // when oke we get this : FE0E670000FFFF80971B01A3D8000007090011
  
      //check if ecu_id_reverse is in the string, then split it there + 2 bytes
      if( strstr(inMess, ecu_id_reverse) )
      {

          tail = split(inMess, ecu_id_reverse + 4);

          if( strstr(tail, "0709") ) 
            {
              if(diagNose) ws.textAll("zigbee running oke");
              return 0;            
            } 
       }     
     
    return 2;
}


//bool ZigbeeLoopBack() {
//    // this test shows if there is a zigbee hardware connected
//    
//    // these commands already have the length 00 and checksum 20 resp 26
//    char loopCmd[11]={"2710AABBCC"}; // ping
//    char s_d[200] = {0}; 
//    //if(diagNose) consoleOut(F("send zb loopback"));
//
//    sendZB( loopCmd ); // answer should be FE02 6101 79 07 1C
////    if ( waitSerial2Available() ) { readZigbee(); } else { readCounter = 0;} // when nothing available we don't read
////    DebugPrintln("inMessage = " + String(inMessage) + " rc = " + String(readCounter));
//
//    char reCeived[254]={0};  
//    strcpy(reCeived, readZB(s_d));
//    
//    if(readCounter > 10) return false;
//    
//    if (strstr(reCeived, "AABBCC" ) == NULL) 
//    {
//       if(diagNose) ws.textAll(F("no loopback answer"));
//       return false;
//    } else {
//       //if(diagNose) consoleOut(F("loopback ok"));
//       return true;
//    }
//}

 void memoryCheck()
 {
 // should we send the heap via mosquitto
    if(Mqtt_Format != 0 && Mqtt_Format != 5 && Mqtt_stateIDX != 0) {
       if (mqttConnect() ) {
            char Mqtt_send[26];
            strcpy( Mqtt_send, Mqtt_outTopic) ;
            if( Mqtt_send[strlen(Mqtt_send)-1] == '/' ) {
                 strcat(Mqtt_send, "heap");
            }
      
        char toMQTT[50]={0}; // when i make this bigger a crash occures
        
        sprintf( toMQTT, "{\"idx\":%d,\"svalue\":\"%ld;%ld\"}", Mqtt_stateIDX, ESP.getFreeHeap(), ESP.getFreeContStack()); 
        //must be like {"idx":901,"nvalue":0,"svalue":"22968;1904"}   length 44
    
        if(diagNose) ws.textAll("mqtt publish heap, mess is : " + String(toMQTT) );
        MQTT_Client.publish ( Mqtt_send, toMQTT, false);  
        
        memset(&toMQTT, 0, sizeof(&toMQTT)); //zero out 
        delayMicroseconds(250);
       }  
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
    //String term = "ZBmodule hard reset" ;
    #ifdef LOG 
    Update_Log(2, "hard reset"); 
    #endif
    if(diagNose) ws.textAll(F("ZBmodule hard reset"));
    delay(2000); //wait for the cc2530 to reboot
    }
