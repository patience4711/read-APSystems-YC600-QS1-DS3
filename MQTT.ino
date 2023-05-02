bool mqttConnect() {   // MQTT connection (documented way from AutoConnect : https://github.com/Hieromon/AutoConnect/tree/master/examples/mqttRSSI_NA)
    // we are here because w'r not connected. Signal with the LED
    ledblink(2,70);
    if ( Mqtt_Broker[0] == '\0' || Mqtt_Broker[0] == '0'  ) 
    {
      Mqtt_Format = 0; // we do not try again
      //DebugPrintln("no broker, cancelling");
      return false;
    }
    //DebugPrint("going to connect to mqtt broker "); DebugPrint(String(Mqtt_Broker));
    //DebugPrintln("connecting to mqtt");
    if (Mqtt_Port == "" ) Mqtt_Port = "1883";   // just in case ....
    uint8_t retry = 3;
//We generate a unique name for this device to avoid mqtt problems 
//in case if you have multiple ESP-ECU devices
    String clientId = String(ESP.getChipId());
    while (!MQTT_Client.connected()) {

      if (MQTT_Client.connect(clientId.c_str() , Mqtt_Username.c_str(), Mqtt_Password.c_str())) 
      {
         //connected so subscribe to inTopic
         MQTT_Client.subscribe ( Mqtt_inTopic.c_str() ) ;   // 
         Update_Log("mqtt", "connected");
         return true;
    } else {
        String term = "connection failed state: " + String(MQTT_Client.state());
        Update_Log("mqtt", term);
        if (!--retry) break; // stop when tried 3 times
        delay(500);
    }
  }
  // if we are here , no connection was made.
  // due to wrong 
  // Shouldn't we prevent that this is tried over and over again?
  // maybe only try to connect at healthcheck?, or once in 20 secs or so?
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
