bool mqttConnect() {   // MQTT connection (documented way from AutoConnect : https://github.com/Hieromon/AutoConnect/tree/master/examples/mqttRSSI_NA)
    // we are here because w'r not connected. Signal with the LED
    if( MQTT_Client.connected() ) {
    if(diagNose) ws.textAll("mqtt was connected");
    return true;
    }
    ledblink(2,70);
//    if ( Mqtt_Broker[0] == '\0' || Mqtt_Broker[0] == '0'  ) 
//    {
//      Mqtt_Format = 0; // we do not try again
//      //DebugPrintln("no broker, cancelling");
//      return false;
//    }
    //DebugPrint("going to connect to mqtt broker "); DebugPrint(String(Mqtt_Broker));
    //DebugPrintln("connecting to mqtt");
    if (Mqtt_Port[0] == '\0' ) strcpy(Mqtt_Port, "1883");   // just in case ....
    uint8_t retry = 3;
    //We generate a unique name for this device to avoid mqtt problems 
    //in case if you have multiple ESP-ECU devices
    //String clientId = String(ESP.getChipId());

   // Serial.println("host=" + String(Mqtt_Broker));
   // Serial.println("Mqtt_Clientid=" + String(Mqtt_Clientid));  
    //Serial.println("Mqtt_Username=" + String(Mqtt_Username));
   // Serial.println("Mqtt_Password=" + String(Mqtt_Password));    
    char Mqtt_inTopic[11]={"ESP-ECU/in"};
    while (!MQTT_Client.connected()) {

      if ( MQTT_Client.connect( Mqtt_Clientid, Mqtt_Username, Mqtt_Password) )
      {
         //connected so subscribe to inTopic
        if(Mqtt_Format != 5 ) {
           if(  MQTT_Client.subscribe ( Mqtt_inTopic ) ) { 
               if(diagNose) ws.textAll("subscribed to " + String(Mqtt_inTopic ));
           }
        }
         if(diagNose) ws.textAll(F("mqtt connected"));
         Update_Log(3, "connected");
 
       
       return true;

    } else {
        //String term = "connection failed state: " + String(MQTT_Client.state());
        Update_Log(3, "failed");
        if (!--retry) break; // stop when tried 3 times
        delay(500);
    }
  }
  // if we are here , no connection was made.
  // due to wrong 
  // Shouldn't we prevent that this is tried over and over again?
  // maybe only try to connect at healthcheck?, or once in 20 secs or so?
  if(diagNose) ws.textAll(F("mqtt connection failed"));
  return false;
}

// *************************************************************************
//                   process received mqtt
// *************************************************************************

void MQTT_Receive_Callback(char *topic, byte *payload, unsigned int length)
{
    //Serial.println("MQTT_message arrived in " + String(Mqtt_inTopic) );
    //Serial.println("length of the message = " + String(length));
    //String SerialRFcmd = "";
   
    String Payload = "";     // convert the payload to a String...
    for (int i = 0; i < length; i++)
    {
        Payload += (char)payload[i]; // convert to char, nodig???
    }
    
    ws.textAll("mqtt received " + Payload);

    StaticJsonDocument<1024> doc;       // We use json library to parse the payload                         
   //  The function deserializeJson() parses a JSON input and puts the result in a JsonDocument.
    DeserializationError error = deserializeJson(doc, Payload); // Deserialize the JSON document

    if (error)            // Test if parsing succeeds.
    {
        ws.textAll("mqtt no valid json ");
        return;
    } 
    
    // We check the kind of command format received with MQTT
    //now we have a payload like {"poll",1}    

    if( doc.containsKey("poll") )
    {
        if(!Polling)
        {
            int inv = doc["poll"].as<int>();  
            //ws.textAll( "found {\"poll\" " + String(inv) + "}\"" );
            
            iKeuze = inv;
            if(iKeuze == 99) {
              ws.textAll( "found {\"poll\" " + String(inv) + "}\"" );
              actionFlag = 48; // takes care for the polling of all inverters
              return;  
            }

            if ( iKeuze < inverterCount ) 
            { 
              actionFlag = 47; // takes care for the polling
              return;
            } else {
               ws.textAll("mqtt error no inv " + String(iKeuze));
              return;         
            }
        }
        else
        {
          ws.textAll("polling = automatic, skipping");
        }
    }
}
