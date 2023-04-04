void start_asyserver() {
DebugPrintln("starting server");
//server.addHandler(&ws);
  server.on("/CONSOLE", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", CONSOLE_HTML);
  });
// Simple Firmware Update
  server.on("/FWUPDATE", HTTP_GET, [](AsyncWebServerRequest *request){
    requestUrl = "/";
    if (!request->authenticate("admin", pswd) ) return request->requestAuthentication();
    request->send_P(200, "text/html", UPDATE_FORM); 
    });
  server.on("/handleFwupdate", HTTP_POST, [](AsyncWebServerRequest *request){
    swap_to_usb(); // this should work now
    Serial.println("FWUPDATE requested");
    if( !Update.hasError() ) {
    toSend="<br><br><center><h2>UPDATE SUCCESS !!</h2><br><br>";
    toSend +="click here to reboot<br><br><a href='/REBOOT'><input style='font-size:3vw;' type='submit' value='REBOOT'></a>";
    } else {
    toSend="<br><br><center><kop>update failed<br><br>";
    toSend +="click here to go back <a href='/FWUPDATE'>BACK</a></center>";
    }
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", toSend);
    response->addHeader("Connection", "close");
    request->send(response);
  
  },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    //Serial.println("filename = " + filename);
    if(filename != "") {
    if(!index){
      //#ifdef DEBUG
        Serial.printf("start firmware update: %s\n", filename.c_str());
      //#endif
      Update.runAsync(true);
      if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
        //#ifdef DEBUG
          Update.printError(Serial);
        //#endif
      }
    }
    } else {
      DebugPrintln("filename empty, aborting");
//     Update.hasError()=true;
    }
    if(!Update.hasError()){
      if(Update.write(data, len) != len){
        //#ifdef DEBUG
          Serial.println("update failed with error: " );
          Update.printError(Serial);
        //#endif
      }
    }
    if(final){
      //#ifdef DEBUG
      if(Update.end(true)){
        Serial.printf("firmware Update Success: %uB\n", index+len);
      } else {
        Update.printError(Serial);
      }
      //#endif
    }
  });
// ***********************************************************************************
//                                     homepage
// ***********************************************************************************
server.on("/SW=BACK", HTTP_GET, [](AsyncWebServerRequest *request) {
    loginBoth(request, "both");
    //loginBoth(request, "both");
//    zendPageHome();
//    request->send(200, "text/html", toSend);
//Serial.println("de requestUrl = " + requestUrl);
request->redirect( requestUrl );
//request->send(302, "text/plane", "");

});

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    loginBoth(request, "both");
    //sendHomepage();
    DebugPrintln("send Homepage");
    request->send_P(200, "text/html", ECU_HOMEPAGE);
});

server.on("/STYLESHEET", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/css", STYLESHEET);
});

server.on("/JAVASCRIPT", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/css", JAVA_SCRIPT);
});
server.on("/INVSCRIPT", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/css", INV_SCRIPT);
});
server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    //Serial.println("favicon requested");
    AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", FAVICON, FAVICON_len);
    request->send(response);
});

server.on("/MENU", HTTP_GET, [](AsyncWebServerRequest *request) {
loginBoth(request, "admin");
toSend = FPSTR(HTML_HEAD);
toSend += FPSTR(MENUPAGE);
request->send(200, "text/html", toSend);
});

// ***********************************************************************************
//                                   basisconfig
// ***********************************************************************************
server.on("/BASISCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
loginBoth(request, "admin");
requestUrl = request->url();// remember this to come back after reboot
zendPageBasis();
request->send(200, "text/html", toSend);
});

server.on("/basisconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
handleBasisconfig(request);
//request->send(200, "text/html", toSend);
request->redirect( requestUrl );
});

server.on("/IPCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
loginBoth(request, "admin");
zendPageIPconfig();
request->send(200, "text/html", toSend);
});

server.on("/IPconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
handleIPconfig(request);
});

server.on("/MQTT", HTTP_GET, [](AsyncWebServerRequest *request) {
loginBoth(request, "admin");
requestUrl = request->url();
zendPageMQTTconfig();
request->send(200, "text/html", toSend);
});

server.on("/MQTTconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
handleMQTTconfig(request);
request->redirect( requestUrl );
});

server.on("/GEOCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
loginBoth(request, "admin");
requestUrl = request->url();
zendPageGEOconfig();
request->send(200, "text/html", toSend);
});

server.on("/geoconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
  //DebugPrintln(F("geoconfig requested"));
handleGEOconfig(request);
request->redirect( requestUrl );
});

server.on("/REBOOT", HTTP_GET, [](AsyncWebServerRequest *request) {
  //DebugPrintln(F("reboot requested"));
  loginBoth(request, "admin");
  actionFlag = 10;
  confirm(); 
  //request->send(200, "text/plain", "ok going to reboot, close this page");
  request->send(200, "text/html", toSend);
});

server.on("/STARTAP", HTTP_GET, [](AsyncWebServerRequest *request) {
        loginBoth(request, "admin");
        //DebugPrintln("We gaan de gegevens wissen");
        String toSend = F("<!DOCTYPE html><html><head><script type='text/javascript'>setTimeout(function(){ window.location.href='/SW=BACK'; }, 5000 ); </script>");
        toSend += F("</head><body><center><h2>OK the accesspoint is started.</h2>Wait unil the led goes on.<br><br>Then go to the wifi-settings on your pc/phone/tablet and connect to ESP-");
        toSend += String(ESP.getChipId()) + " !";
        request->send ( 200, "text/html", toSend ); //zend bevestiging
        actionFlag = 11;
        //request->redirect("/");
});

server.on("/INFOPAGE", HTTP_GET, [](AsyncWebServerRequest *request) {
//Serial.println(F("/INFOPAGE requested"));
loginBoth(request, "both");
handleInfo(request);
});

server.on("/TEST", HTTP_GET, [](AsyncWebServerRequest *request) {
loginBoth(request, "admin");
actionFlag = 44;
request->send( 200, "text/html", "<center><br><br><h3>checking zigbee.. please wait a minute.<br>Then you can find the result in the log.<br><br><a href=\'/LOGPAGE\'>click here</a></h3>" );
});

server.on("/LOGPAGE", HTTP_GET, [](AsyncWebServerRequest *request) {
loginBoth(request, "both");
requestUrl = request->url();
zendPageLog();
request->send( 200, "text/html", toSend );
});

server.on("/CLEAR_LOG", HTTP_GET, [](AsyncWebServerRequest *request) {
  loginBoth(request, "admin");
  //requestUrl = request->url();
  DebugPrintln(F("clear log requested"));
  Clear_Log();
  //toSend = FPSTR(CONFIRM);
  //request->send(200, "text/html", toSend); //send the html code to the client  
  request->redirect( requestUrl ); // refreshes the page
});

server.on("/MQTT_TEST", HTTP_GET, [](AsyncWebServerRequest *request) {
loginBoth(request, "admin");

String Mqtt_send = Mqtt_outTopic;
if(Mqtt_outTopic.endsWith("/")) {
//String suffix = String(Inv_Prop[which].invIdx);
  //Mqtt_send += suffix;
  Mqtt_send += String(Inv_Prop[0].invIdx);
}

String toMQTT = "{\"test\":\"" + Mqtt_send + "\"}";
//DebugPrintln("MQTT_Client.publish the message : " + toMQTT);
MQTT_Client.publish ( Mqtt_send.c_str(), toMQTT.c_str(), true );
toSend = "sent mqtt message : " + toMQTT;
request->send( 200, "text/plain", toSend  );
});
  
// ********************************************************************
//                    inverters
// ******************************************************************
server.on("/PAIR", HTTP_GET, [](AsyncWebServerRequest *request) {
  loginBoth(request, "admin");
  requestUrl = request->url();
  //DebugPrintln(F("pairing requested"));
  handlePair(request);
});

server.on("/INV_DEL", HTTP_GET, [](AsyncWebServerRequest *request) {
// save the data
//requestUrl = request->url();
handleInverterdel(request);
});

server.on("/inverterconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
//requestUrl = "/";
handleInverterconfig(request);
});

server.on("/INV_CONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
iKeuze=0;
//Serial.println("test requested, ikeuze = " + String(iKeuze) );
    //Serial.println("Inv_Prop[iKeuze].invID = " + String(Inv_Prop[iKeuze].invID));
    inverterForm(); // prepare the page part with the form
    request->send_P(200, "text/html", INVCONFIG_START, processor);
    DebugPrintln("sendInverterconfig done");
});

server.on("/INV", HTTP_GET, [](AsyncWebServerRequest *request) {
    requestUrl = "/";
    //bool nothing = false;
    int i = atoi(request->arg("welke").c_str()) ;
    iKeuze = i;
    //DebugPrintln("?INV iKeuze at enter = " + String(iKeuze));
    if( iKeuze == 99 ) {
      iKeuze = inverterCount; //indicate this is an adition
      inverterCount += 88;
     }
     //DebugPrint("iKeuze is "); //DebugPrintln(iKeuze);
     //Serial.println("Inv_Prop[iKeuze].invID = " + String(Inv_Prop[iKeuze].invID));
     String bestand = "/Inv_Prop" + String(iKeuze) + ".str";
     if (!LittleFS.exists(bestand)) Inv_Prop[iKeuze].invType = 2;
    //nu roepen we zendpageRelevant aan
    inverterForm(); // prepare the form page
    request->send_P(200, "text/html", INVCONFIG_START, processor); //send the html code to the client
});

server.on("/CONFIRM_INV", HTTP_GET, [](AsyncWebServerRequest *request) {
    toSend = FPSTR(CONFIRM_INV); // prepare the page
    request->send(200, "text/html", toSend); //send the html code to the client
});
// ********************************************************************
//                    X H T  R E Q U E S T S
//***********************************************************************


server.on("/get.currentTime", HTTP_GET, [](AsyncWebServerRequest *request) {
     //ledblink(1, 100);
     String uur = String(hour());
     if(hour() < 10) { uur = "0" + String(hour()); } 
      String minuten = String(minute());
      if(minute() < 10) { minuten = "0" + String(minute()); }
     String json = "{";
     json += "\"uur\":\"" + uur + "\",\"min\":\"" + minuten + "\"}";
     request->send(200, "text/json", json);
     json = String();
});

server.on("/get.Times", HTTP_GET, [](AsyncWebServerRequest *request) {
     String json = "{";
     // start polling
     String ssuur = "0" + String(hour(switchonTime));
     String ssmin = String(minute(switchonTime));
     if( minute(switchonTime) < 10 ) ssmin = "0" + ssmin;
     ssuur += ":" + ssmin;
     json += "\"srt\":\"" + ssuur + " hr\"";
     // end polling
     ssuur = String(hour(switchoffTime));
     ssmin = String(minute(switchoffTime));
     if( minute(switchoffTime) < 10 ) ssmin = "0" + ssmin;
     ssuur += ":" + ssmin;
     json += ",\"sst\":\"" + ssuur + " hr\"}";

     request->send(200, "text/json", json);
     json = String();
});

//server.on("/get.Count", HTTP_GET, [](AsyncWebServerRequest *request) {     
//// set the array into a json object
//  String json="{";
//      json += "\"count\":\"" + String(inverterCount) + "\"";
//      json += "}";     
//     request->send(200, "text/json", json);
//     json = String();
//});

server.on("/get.Inverter", HTTP_GET, [](AsyncWebServerRequest *request) {     
// set the array into a json object
  String json;
  int panelCount=2;
  int i = (request->arg("inv").toInt()) ;
  //Serial.println("i = " + String(i));
  if( i < inverterCount) {
      json="{";

      if(Inv_Prop[i].invType == 1) panelCount=4;
      
      json += "\"serial\":\"" + String(Inv_Prop[i].invSerial)  + "\"";      
      json += ",\"freq\":"  + String(atof(Inv_Data[i].freq)); //freq = a char    
      json += ",\"temp\":"  + String(atof(Inv_Data[i].heath));
      json += ",\"acv\":"   + String(atof(Inv_Data[i].acv));

      for(int z = 0; z < panelCount; z++ ) 
      {
        json += ",\"dcv" + String(z) + "\":" + String(atof(Inv_Data[i].dcv[z]));
        json += ",\"dcc" + String(z) + "\":" + String(atof(Inv_Data[i].dcc[z]));
        json += ",\"pow" + String(z) + "\":" + String(atof(Inv_Data[i].power[z]));
        json += ",\"en" + String(z) + "\":" + String(en_saved[i][z], 2);
      }
      json += ",\"power\":" + String(atof(Inv_Data[i].power[4]));
      json += ",\"energy\":" + String(Inv_Data[i].en_total, 2);
      json += "}";     

     request->send(200, "text/json", json);
     json = String();
     } else {
     String term = "unknown inverter " + String(i);
     request->send(200, "text/plain", term);
     }
});
    
// this link provides the inverterdata on the frontpage
server.on("/get.Power", HTTP_GET, [](AsyncWebServerRequest *request) {     
// set the array into a json object
#ifdef TEST
zigbeeUp = 1;
#endif

String json;
int i = atoi(request->arg("inv").c_str()) ;
// inv prop comes from spiffs so if we could evaluate that it exists
      json="{";
      json += "\"nm\":\"" + String(Inv_Prop[i].invLocation) + "\"";
      json += ",\"polled\":\"" + String(polled[i]) + "\"";    
      json += ",\"type\":\"" + String(Inv_Prop[i].invType) + "\"";
      json += ",\"count\":\"" + String(inverterCount) + "\"";
      
// first test if the inverter exists
if(String(Inv_Prop[i].invLocation) != "N/A") {
      for(int z = 0; z < 4; z++ ) 
      {
         if(Inv_Prop[i].conPanels[z]) // is this panel connected?
         {
             json += ",\"p" +  String(z) + "\":\"" + Inv_Data[i].power[z] + "\"";  
         }   else {
             json += ",\"p" +  String(z) + "\":\"n/e\""; //when no panel
         }
      }
     json += ",\"eN\":\"" + String(Inv_Data[i].en_total/(float)1000, 2) + "\""; 
//     json += ",\"eN2\":\"" + String((en_saved[i][0] + en_saved[i][1]), 2) + "\"";//energy per panel
     //json += ",\"state\":\"" + String(zigbeeUp) + "\"";
 } 
    else
 { // invSerial="000000000000" so we make all values n/e

      for(int z = 0; z < 4; z++ ) 
      {
        json += ",\"p" +  String(z) + "\":\"n/e\""; 
      }
     json += ",\"eN\":\"n/e\"";
     
 }
     json += ",\"state\":\"" + String(zigbeeUp) + "\""; 
 
     uint8_t night = 0; 
     if(!timeRetrieved ) { night=0; } else {
     //if(now() > switchonTime && now() < switchoffTime )
     if(dayTime) { night = 0; } else { night = 1; } 
     }
     json += ",\"sleep\":\"" + String(night) + "\"";
     json += "}";     
    
    //Serial.println("get.Power reaction = " + json);
     request->send(200, "text/json", json);
     json = String();
 
});

server.on("/get.Paired", HTTP_GET, [](AsyncWebServerRequest *request) {     
// set the array into a json object
String json="{";
     //Serial.println("iKeuze = " + String(iKeuze));
     //Serial.println("json Inv_Prop[" + String(iKeuze) + "].invID = " + String(Inv_Prop[iKeuze].invID));      
      json += "\"invID\":\"" + String(Inv_Prop[iKeuze].invID) + "\"";
      json += "}";
     //Serial.println("get.Paired reaction = " + json);
     request->send(200, "text/json", json);
     json = String();
});

// if everything failed we come here
server.onNotFound([](AsyncWebServerRequest *request){
  //Serial.println("unknown request");
  handleNotFound(request);
});

server.begin(); 
}

void confirm() {
toSend="<html><body onload=\"setTimeout(function(){window.location.href='/';}, 3000 );\"><br><br><center><h3>processing<br>your request,<br>please wait</html>";
}
