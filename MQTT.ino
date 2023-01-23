bool mqttConnect() {   // MQTT connection (documented way from AutoConnect : https://github.com/Hieromon/AutoConnect/tree/master/examples/mqttRSSI_NA)
    if ( Mqtt_Broker[0] == '\0' || Mqtt_Broker[0] == '0'  ) 
    {
      Mqtt_Format = 0; // we proberen het niet opnieuw
      //DebugPrintln("no broker, cancelling");
      return false;
    }
    //DebugPrint("going to connect to mqtt broker "); DebugPrint(String(Mqtt_Broker));
    //DebugPrintln("connecting to mqtt");
    if (Mqtt_Port == "" ) Mqtt_Port = "1883";   // just in case ....
    uint8_t retry = 3;
    String clientId = String(ESP.getChipId());
    DebugPrintln("\nAttempting MQTT connect.. ");
    while (!MQTT_Client.connected()) {

     //MQTT_Client.setServer(Mqtt_Broker.c_str(), Mqtt_Port.toInt());


      //We generate a unique name for this device to avoid mqtt problems in case if you have multiple RFlink-ESP devices
      //String clientId = Adv_HostName + "-" + String(GET_CHIPID(), HEX);
      //String clientId = String(ESP.getChipId());
      if (MQTT_Client.connect(clientId.c_str() , Mqtt_Username.c_str(), Mqtt_Password.c_str())) 
      {
        //DebugPrint(F("MQTT connection Established with ID : ")); //+ String(clientId));
        //DebugPrintln(clientId);
        // ... and subscribe, send = MQTTtoRF
        //String topic = Mqtt_Topic;
         MQTT_Client.subscribe ( Mqtt_inTopic.c_str() ) ;   // 
         //DebugPrint("MQTT subscribed on topic "); //DebugPrintln(Mqtt_inTopic);
         Update_Log("mqtt", "connected");
         return true;
    } else {
        String term = "connection failed state: " + String(MQTT_Client.state());
        Update_Log("mqtt", term);
        ////DebugPrintln("Connection mqttserver:" + String(Mqtt_Broker));
        ////DebugPrintln("Connection failed:" + String(MQTT_Client.state()));
              if (!--retry) break;
      delay(500);
    }
  }
  //Mqtt_Enabled = false; // we proberen het niet opnieuw
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
