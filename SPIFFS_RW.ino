// ******************   spiffs lezen  *************************

// als er geen spiffs bestand is dan moet hij eigenlijk altijd een ap openenen
void SPIFFS_read() {
  //DebugPrintln("mounting FS...");
 if (LittleFS.begin()) {
    //DebugPrintln("mounted file system");

       if( file_open_for_read("/wificonfig.json") ) {
                Serial.println("read wificonfig\n");
          } else {
             Serial.println("wificonfig.json not opened\n");
          }
       
       if( file_open_for_read("/basisconfig.json") ) {     
             Serial.println("read basisconfig\n");
          } else {
          Serial.println("basisconfig.json not opened\n");
        } 
       if( file_open_for_read("/mqttconfig.json") ) {     
             Serial.println("mqttconfig read");
          } else {
          Serial.println("mqttconfig.json not opened");
        }         
  } 
 // einde spiffs lezen 3 bestanden

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

      Serial.print(F( "leesStruct ivn = ")); Serial.println( String(ivn) );  
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
   //DebugPrintln("saving config");

    DynamicJsonDocument doc(1024);
    JsonObject json = doc.to<JsonObject>();   
    json["ip"] = static_ip;
    json["pswd"] = pswd;
    json["longi"] = longi;
    json["lati"] = lati;
    json["gmtOffset"] = gmtOffset;
    json["zomerTijd"] = zomerTijd;
    json["securityLevel"] = securityLevel;
    File configFile = LittleFS.open("/wificonfig.json", "w");
//    if (!configFile) {
//      //DebugPrintln("open file for writing failed!");
//    }
    //DebugPrintln("wificonfig.json written");
//    #ifdef DEBUG 
////    json.printTo(Serial);
    serializeJson(json, Serial);
    Serial.println(F("")); 
//    #endif
    serializeJson(json, configFile);
    configFile.close();
}


void basisConfigsave() {
    //DebugPrintln("saving basis config");
    DynamicJsonDocument doc(1024);
    JsonObject json = doc.to<JsonObject>();
    json["ECU_ID"] = ECU_ID;
    json["userPwd"] = userPwd;
    json["inverterCount"] = inverterCount;
    json["Polling"] = Polling;
    json["pollOffset"] = pollOffset;
    File configFile = LittleFS.open("/basisconfig.json", "w");
    if (!configFile) {
      //DebugPrintln("open file for writing failed");
    }
 //   DebugPrintln("inverterconfig.json written");
//    #ifdef DEBUG 
//    serializeJson(json, Serial);
//    Serial.println(F(""));     
//    #endif
    serializeJson(json, configFile);
    configFile.close();
}

void mqttConfigsave() {
   //DebugPrintln("saving mqtt config");
    DynamicJsonDocument doc(1024);
    JsonObject json = doc.to<JsonObject>();
// 

    json["Mqtt_Broker"] = Mqtt_Broker;
    json["Mqtt_Port"] = Mqtt_Port;    
    json["Mqtt_stateIDX"] = Mqtt_stateIDX;
    json["Mqtt_outTopic"] = Mqtt_outTopic;
    json["Mqtt_Username"] = Mqtt_Username;
    json["Mqtt_Password"] = Mqtt_Password;
    json["Mqtt_Clientid"] = Mqtt_Clientid;    
    json["Mqtt_Format"] = Mqtt_Format;    
    File configFile = LittleFS.open("/mqttconfig.json", "w");
    if (!configFile) {
      //DebugPrintln("open file for writing failed");
    }
    //DebugPrintln("mqttconfig.json written");
//    #ifdef DEBUG
//    serializeJson(json, Serial);
//    Serial.println(F("")); 
//    #endif
    serializeJson(json, configFile);
    configFile.close();
}


bool file_open_for_read(String bestand) {
      //DebugPrint("we are in file_open_for_read, bestand = "); //DebugPrintln(bestand); 
      if (!LittleFS.exists(bestand)) return false;
      
      //file exists, reading and loading
      //DebugPrintln("bestand bestaat");
        File configFile = LittleFS.open(bestand, "r");
        if (!configFile) return false;
        
        //DebugPrint("opened config file"); //DebugPrintln(bestand);
           size_t size = configFile.size();
          // Allocate a buffer to store contents of the file.
           std::unique_ptr<char[]> buf(new char[size]);
           configFile.readBytes(buf.get(), size);
           DynamicJsonDocument doc(1024);
           auto error = deserializeJson(doc, buf.get());
//           #ifdef DEBUG 
//           serializeJson(doc, Serial); Serial.println(F(""));
//           #endif
             if (error) return false; 
              //DebugPrintln("parsed json");
              String jsonStr = ""; // we printen het json object naar een string
            // nu kunnen we eerst controleren of een bepaalde entry bestaat
            // zoniet slaan we die over anders crasht de ESP
                serializeJson(doc, jsonStr);
            if (bestand == "/wificonfig.json") {
                      if(jsonStr.indexOf("ip") > 0){ strcpy(static_ip, doc["ip"]);}
                      if(jsonStr.indexOf("pswd") > 0){ strcpy(pswd, doc["pswd"]);}
//                      if(jsonStr.indexOf("lengte") > 0){ strcpy(lengte, doc["lengte"]);}
//                      if(jsonStr.indexOf("breedte") > 0){ strcpy(breedte, doc["breedte"]);}
                      if(jsonStr.indexOf("longi") > 0){longi = doc["longi"].as<float>();}
                      if(jsonStr.indexOf("lati") > 0){lati = doc["lati"].as<float>();}                      
                      if(jsonStr.indexOf("gmtOffset") > 0){ strcpy(gmtOffset, doc["gmtOffset"]);}
                      if(jsonStr.indexOf("zomerTijd") > 0){zomerTijd = doc["zomerTijd"].as<bool>();}
                      if(jsonStr.indexOf("securityLevel") > 0){securityLevel = doc["securityLevel"].as<int>();}
            }

            if (bestand == "/basisconfig.json") {
                    if(jsonStr.indexOf("ECU_ID") > 0) { strcpy (ECU_ID, doc["ECU_ID"] );}
                    if(jsonStr.indexOf("userPwd") > 0) { strcpy (userPwd, doc["userPwd"] );}
  //                  if(jsonStr.indexOf("pollRes") > 0) {pollRes = doc["pollRes"].as<int>();}
                    if(jsonStr.indexOf("pollOffset") > 0) {pollOffset = doc["pollOffset"].as<int>();}
                    if(jsonStr.indexOf("inverterCount") > 0) {inverterCount = doc["inverterCount"].as<int>();}
                    if(jsonStr.indexOf("Polling") > 0) {Polling = doc["Polling"].as<bool>();}
                   // if(jsonStr.indexOf("calli") > 0) {calliBration = doc["calli"].as<float>();}
              }            

            if (bestand == "/mqttconfig.json"){
                     if(jsonStr.indexOf("Mqtt_Broker")   >  0 ) { strcpy(Mqtt_Broker,   doc["Mqtt_Broker"])         ;}
                     if(jsonStr.indexOf("Mqtt_Port")     >  0 ) { strcpy(Mqtt_Port,     doc["Mqtt_Port"])           ;}  
                    // if(jsonStr.indexOf("Mqtt_inTopic")  >  0 ) { strcpy(Mqtt_inTopic,  doc["Mqtt_inTopic"])        ;}         
                     if(jsonStr.indexOf("Mqtt_outTopic") >  0 ) { strcpy(Mqtt_outTopic, doc["Mqtt_outTopic"])       ;}         
                     if(jsonStr.indexOf("Mqtt_Username") >  0 ) { strcpy(Mqtt_Username, doc["Mqtt_Username"])       ;}
                     if(jsonStr.indexOf("Mqtt_Password") >  0 ) { strcpy(Mqtt_Password, doc["Mqtt_Password"])       ;}
                     if(jsonStr.indexOf("Mqtt_Clientid") >  0 ) { strcpy(Mqtt_Clientid, doc["Mqtt_Clientid"])       ;}
                     if(jsonStr.indexOf("Mqtt_Format")   >  0 ) { Mqtt_Format =         doc["Mqtt_Format"].as<int>();}
                     if(jsonStr.indexOf("Mqtt_stateIDX")   >  0 ) { Mqtt_stateIDX =         doc["Mqtt_stateIDX"].as<int>();}
            }
             return true;
 }

// we do this before swap_to_zigbee
//void printStruct( String bestand ) {
////input String bestand = "/Inv_Prop" + String(x) + ".str";
//      //String bestand = bestand + String(i) + ".str"
//      //leesStruct(bestand); is done at boottime
////      #ifdef DEBUG
////      int ivn = bestand.substring(9,10).toInt();
////      Serial.println("Inv_Prop[" + String(ivn) + "].invLocation = " + String(Inv_Prop[ivn].invLocation));     
////      Serial.println("Inv_Prop[" + String(ivn) + "].invSerial = " + String(Inv_Prop[ivn].invSerial));
////      Serial.println("Inv_Prop[" + String(ivn) + "].invID = " + String(Inv_Prop[ivn].invID)); 
////      Serial.println("Inv_Prop[" + String(ivn) + "].invType = " + String(Inv_Prop[ivn].invType));
////      Serial.println("Inv_Prop[" + String(ivn) + "].invIdx = " + String(Inv_Prop[ivn].invIdx));
////      Serial.println("Inv_Prop[" + String(ivn) + "].conPanels = " + String(Inv_Prop[ivn].conPanels[0])  + String(Inv_Prop[ivn].conPanels[1]) + String(Inv_Prop[ivn].conPanels[2]) + String(Inv_Prop[ivn].conPanels[3]));      
////      Serial.println("");
////      Serial.println("****************************************");
////      #endif
//}
