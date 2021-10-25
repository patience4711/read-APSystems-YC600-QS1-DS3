// ******************   spiffs lezen  *************************

// als er geen spiffs bestand is dan moet hij eigenlijk altijd een ap openenen
void SPIFFS_read() {
  //DebugPrintln("mounting FS...");
 if (LittleFS.begin()) {
    DebugPrintln("mounted file system");

       if( file_open_for_read("/wificonfig.json") ) {
                DebugPrintln("read wificonfig\n");
          } else {
             DebugPrintln("wificonfig.json not opened\n");
          }
       
       if( file_open_for_read("/basisconfig.json") ) {     
             DebugPrintln("read basisconfig\n");
          } else {
          DebugPrintln("basisconfig.json not opened\n");
        } 
       if( file_open_for_read("/mqttconfig.json") ) {     
             DebugPrintln("mqttconfig read");
          } else {
          DebugPrintln("mqttconfig.json not opened");
        }         
  } else {
        DebugPrintln("failed to mount FS");
   }
 // einde spiffs lezen 5 bestanden

}  

void writeStruct( String whichfile, int nummer) {
      File configFile = LittleFS.open(whichfile, "w");
      //input=/Inv_Prop2.str
     //int nummer=whichfile.substring(9,10).toInt();
     //Serial.println("nummer = "+ String(nummer));  
        if (!configFile)
           {
             Serial.print(F("Failed open for write : ")); Serial.println(whichfile);            
           } 
           
              Serial.print(F("Opened for write....")); Serial.println(whichfile);
              configFile.write( (unsigned char *)&Inv_Prop[nummer], sizeof(Inv_Prop[nummer]) );
              configFile.close();
           
}

bool leesStruct(String whichfile) {
      //input = /Inv_Prop0.str
      File configFile = LittleFS.open(whichfile, "r");
      int ivn = whichfile.substring(9,10).toInt();

      Serial.println( "leesStruct ivn = " + String(ivn) );  
        if (!configFile)
           {
              Serial.print(F("Failed to open for read")); Serial.println(whichfile);
              return false;           
           } 
              Serial.print(F("reading ")); Serial.println(whichfile);
              configFile.read( (unsigned char *)&Inv_Prop[ivn], sizeof(Inv_Prop[ivn]) );
              configFile.close();
              return true;
 }



// **************************************************************************** 
//                      de gegevens opslaan in SPIFFS                         *  
// ****************************************************************************
void wifiConfigsave() {
   DebugPrintln("saving config");

    DynamicJsonDocument doc(1024);
    JsonObject json = doc.to<JsonObject>();   
    json["ip"] = static_ip;
    json["pswd"] = pswd;
    json["lengte"] = lengte;
    json["breedte"] = breedte;
    json["timezone"] = timezone;
    json["zomerTijd"] = zomerTijd;

    File configFile = LittleFS.open("/wificonfig.json", "w");
    if (!configFile) {
      DebugPrintln("open file for writing failed!");
    }
    //DebugPrintln("wificonfig.json written");
    #ifdef DEBUG 
//    json.printTo(Serial);
    serializeJson(json, Serial);
    Serial.println(""); 
    #endif
    serializeJson(json, configFile);
    configFile.close();
}


void basisConfigsave() {
    DebugPrintln("saving basis config");
    DynamicJsonDocument doc(1024);
    JsonObject json = doc.to<JsonObject>();
    json["ECU_ID"] = ECU_ID;
    json["userPwd"] = userPwd;
    json["pollRes"] = pollRes;
    json["inverterCount"] = inverterCount;
    json["Polling"] = Polling;
    json["pollOffset"] = pollOffset;
    File configFile = LittleFS.open("/basisconfig.json", "w");
    if (!configFile) {
      //DebugPrintln("open file for writing failed");
    }
    DebugPrintln("inverterconfig.json written");
    #ifdef DEBUG 
    serializeJson(json, Serial);
    Serial.println("");     
    #endif
    serializeJson(json, configFile);
    configFile.close();
}

void mqttConfigsave() {
   //DebugPrintln("saving mqtt config");
    DynamicJsonDocument doc(1024);
    JsonObject json = doc.to<JsonObject>();
// 
    json["Mqtt_Enabled"] = Mqtt_Enabled;
    json["Mqtt_Broker"] = Mqtt_Broker;
    json["Mqtt_Port"] = Mqtt_Port;    
    json["Mqtt_inTopic"] = Mqtt_inTopic;
    json["Mqtt_outTopic"] = Mqtt_outTopic;
    json["Mqtt_Username"] = Mqtt_Username;
    json["Mqtt_Password"] = Mqtt_Password;
//    json["Mqtt_Idx"] = Mqtt_Idx;    
    File configFile = LittleFS.open("/mqttconfig.json", "w");
    if (!configFile) {
      //DebugPrintln("open file for writing failed");
    }
    DebugPrintln("mqttconfig.json written");
    #ifdef DEBUG
    serializeJson(json, Serial);
    Serial.println(""); 
    #endif
    serializeJson(json, configFile);
    configFile.close();
}


bool file_open_for_read(String bestand) {
      //DebugPrint("we are in file_open_for_read, bestand = "); //DebugPrintln(bestand); 
      if (LittleFS.exists(bestand)) {
      //file exists, reading and loading
      //DebugPrintln("bestand bestaat");
        File configFile = LittleFS.open(bestand, "r");
        if (configFile) {
           //DebugPrint("opened config file"); //DebugPrintln(bestand);
           size_t size = configFile.size();
          // Allocate a buffer to store contents of the file.
           std::unique_ptr<char[]> buf(new char[size]);
           configFile.readBytes(buf.get(), size);
           DynamicJsonDocument doc(1024);
           auto error = deserializeJson(doc, buf.get());
           #ifdef DEBUG 
           serializeJson(doc, Serial); Serial.println("");
           #endif
             if (!error) {
              //DebugPrintln("parsed json");
              String jsonStr = ""; // we printen het json object naar een string
            // nu kunnen we eerst controleren of een bepaalde entry bestaat
            // zoniet slaan we die over anders crasht de ESP
                serializeJson(doc, jsonStr);
            if (bestand == "/wificonfig.json") {
                      if(jsonStr.indexOf("ip") > 0){ strcpy(static_ip, doc["ip"]);}
                      if(jsonStr.indexOf("pswd") > 0){ strcpy(pswd, doc["pswd"]);}
                      if(jsonStr.indexOf("lengte") > 0){ strcpy(lengte, doc["lengte"]);}
                      if(jsonStr.indexOf("breedte") > 0){ strcpy(breedte, doc["breedte"]);}
                      if(jsonStr.indexOf("timezone") > 0){ strcpy(timezone, doc["timezone"]);}
                      if(jsonStr.indexOf("zomerTijd") > 0){zomerTijd = doc["zomerTijd"].as<bool>();}
            }

            if (bestand == "/basisconfig.json") {
                    if(jsonStr.indexOf("ECU_ID") > 0) { strcpy (ECU_ID, doc["ECU_ID"] );}
                    if(jsonStr.indexOf("userPwd") > 0) { strcpy (userPwd, doc["userPwd"] );}
                    if(jsonStr.indexOf("pollRes") > 0) {pollRes = doc["pollRes"].as<int>();}
                    if(jsonStr.indexOf("pollOffset") > 0) {pollOffset = doc["pollOffset"].as<int>();}
                    if(jsonStr.indexOf("inverterCount") > 0) {inverterCount = doc["inverterCount"].as<int>();}
                    if(jsonStr.indexOf("Polling") > 0) {Polling = doc["Polling"].as<bool>();}
              }            

            if (bestand == "/mqttconfig.json"){
                     if(jsonStr.indexOf("Mqtt_Enabled") > 0)  { Mqtt_Enabled =  doc["Mqtt_Enabled"].as<bool>();}   
                     if(jsonStr.indexOf("Mqtt_Broker") > 0)   { Mqtt_Broker =   doc["Mqtt_Broker"].as<String>();}
                     if(jsonStr.indexOf("Mqtt_Port") > 0)     { Mqtt_Port =   doc["Mqtt_Port"].as<String>();}  
                     if(jsonStr.indexOf("Mqtt_inTopic") > 0)  { Mqtt_inTopic =  doc["Mqtt_inTopic"].as<String>();}         
                     if(jsonStr.indexOf("Mqtt_outTopic") > 0) { Mqtt_outTopic = doc["Mqtt_outTopic"].as<String>();}         
                     if(jsonStr.indexOf("Mqtt_Username") > 0) { Mqtt_Username = doc["Mqtt_Username"].as<String>();}
                     if(jsonStr.indexOf("Mqtt_Password") > 0) { Mqtt_Password = doc["Mqtt_Password"].as<String>();}
                     //if(jsonStr.indexOf("Mqtt_Idx") > 0) { Mqtt_Idx = doc["Mqtt_Idx"].as<int>();}
            
            }
             return true;
           } else {
            return false;
           }
       }
   }
}
// we do this before swap_to_zigbee
void printStruct( String bestand ) {
//input String bestand = "/Inv_Prop" + String(x) + ".str";
      //String bestand = bestand + String(i) + ".str"
      //leesStruct(bestand); is done at boottime
      int ivn = bestand.substring(9,10).toInt();
      Serial.println("Inv_Prop[" + String(ivn) + "].invLocation = " + String(Inv_Prop[ivn].invLocation));     
      Serial.println("Inv_Prop[" + String(ivn) + "].invSerial = " + String(Inv_Prop[ivn].invSerial));
      Serial.println("Inv_Prop[" + String(ivn) + "].invID = " + String(Inv_Prop[ivn].invID)); 
      Serial.println("Inv_Prop[" + String(ivn) + "].invType = " + String(Inv_Prop[ivn].invType));
      Serial.println("Inv_Prop[" + String(ivn) + "].invIdx = " + String(Inv_Prop[ivn].invIdx));
      Serial.println("Inv_Prop[" + String(ivn) + "].conPanels = " + String(Inv_Prop[ivn].conPanels[0])  + String(Inv_Prop[ivn].conPanels[1]) + String(Inv_Prop[ivn].conPanels[2]) + String(Inv_Prop[ivn].conPanels[3]));      
      Serial.println("");
      Serial.println("****************************************");
      
}
