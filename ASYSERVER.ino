void start_asyserver() {
DebugPrintln("starting server");
//server.addHandler(&ws);

//server.on("/details", HTTP_GET, [](AsyncWebServerRequest *request) {
//int i = atoi(request->arg("inv").c_str()) ;
//requestUrl = request->url();
//sendPageDetails(i);
//request->send(200, "text/html", toSend);
//});
server.on("/details", HTTP_GET, [](AsyncWebServerRequest *request) {
iKeuze = atoi(request->arg("inv").c_str()) ; // so we know for which inverter
requestUrl = request->url();
//sendPageDetails(i);
request->send_P(200, "text/html", DETAILSPAGE);
});

server.on("/CONSOLE", HTTP_GET, [](AsyncWebServerRequest *request){
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    request->send_P(200, "text/html", CONSOLE_HTML);
  });

// Simple Firmware Update
  server.on("/FWUPDATE", HTTP_GET, [](AsyncWebServerRequest *request){
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );    
    requestUrl = "/";
    if (!request->authenticate("admin", pswd) ) return request->requestAuthentication();
    request->send_P(200, "text/html", UPDATE_FORM); 
    });
  server.on("/handleFwupdate", HTTP_POST, [](AsyncWebServerRequest *request){
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    swap_to_usb(); // this should work now
    Serial.println(F("FWUPDATE requested"));
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
          Serial.println(F("update failed with error: " ));
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
   request->redirect( requestUrl );
//request->send(302, "text/plane", "");

});

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    loginBoth(request, "both");
    //DebugPrintln("send Homepage");
    request->send_P(200, "text/html", ECU_HOMEPAGE);
});

server.on("/STYLESHEET", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/css", STYLESHEET);
});
server.on("/JAVASCRIPT", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/css", JAVA_SCRIPT);
});
server.on("/SECURITY", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/css", SECURITY);
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
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    request->send_P(200, "text/html", MENUPAGE);
});
server.on("/DENIED", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/html", REQUEST_DENIED);
});
// ***********************************************************************************
//                                   basisconfig
// ***********************************************************************************
server.on("/BASISCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
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
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    zendPageIPconfig();
    request->send(200, "text/html", toSend);
});

server.on("/IPconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleIPconfig(request);
});

server.on("/MQTT", HTTP_GET, [](AsyncWebServerRequest *request) {
     if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
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
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
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
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    actionFlag = 10;
    confirm(); 
    request->send(200, "text/html", toSend);
});

server.on("/STARTAP", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    //DebugPrintln("We gaan de gegevens wissen");
    String toSend = F("<!DOCTYPE html><html><head><script type='text/javascript'>setTimeout(function(){ window.location.href='/SW=BACK'; }, 5000 ); </script>");
    toSend += F("</head><body><center><h2>OK the accesspoint is started.</h2>Wait unil the led goes on.<br><br>Then go to the wifi-settings on your pc/phone/tablet and connect to ESP-");
    toSend += String(ESP.getChipId()) + " !";
    request->send ( 200, "text/html", toSend ); //zend bevestiging
    actionFlag = 11;
});

//server.on("/INFOPAGE", HTTP_GET, [](AsyncWebServerRequest *request) {
//    //Serial.println(F("/INFOPAGE requested"));
//    loginBoth(request, "both");
//    handleInfo(request);
//});
server.on("/ABOUT", HTTP_GET, [](AsyncWebServerRequest *request) {
    //Serial.println(F("/INFOPAGE requested"));
    loginBoth(request, "both");
    handleAbout(request);
});
server.on("/TEST", HTTP_GET, [](AsyncWebServerRequest *request) {
     if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
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
      if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
      loginBoth(request, "admin");
      Clear_Log();
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
      if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
      loginBoth(request, "admin");
      requestUrl = request->url();
      //DebugPrintln(F("pairing requested"));
      handlePair(request);
});

server.on("/INV_DEL", HTTP_GET, [](AsyncWebServerRequest *request) {
      if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
      handleInverterdel(request);
});

server.on("/inverterconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    handleInverterconfig(request);
});

server.on("/INV_CONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    iKeuze=0;
    inverterForm(); // prepare the page part with the form
    request->send_P(200, "text/html", INVCONFIG_START, processor);
    DebugPrintln("sendInverterconfig done");
});

server.on("/INV", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
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

//server.on("/get.Times", HTTP_GET, [](AsyncWebServerRequest *request) {
//     String json = "{";
//     // start polling
//     String ssuur = "0" + String(hour(switchonTime));
//     String ssmin = String(minute(switchonTime));
//     if( minute(switchonTime) < 10 ) ssmin = "0" + ssmin;
//     ssuur += ":" + ssmin;
//     json += "\"srt\":\"" + ssuur + " hr\"";
//     // end polling
//     ssuur = String(hour(switchoffTime));
//     ssmin = String(minute(switchoffTime));
//     if( minute(switchoffTime) < 10 ) ssmin = "0" + ssmin;
//     ssuur += ":" + ssmin;
//     json += ",\"sst\":\"" + ssuur + " hr\"}";
//
//     request->send(200, "text/json", json);
//     json = String();
//});

server.on("/get.Times", HTTP_GET, [](AsyncWebServerRequest *request) {
  //{"srt":"05:37 hr","sst":"21:32 hr"}
     char json[40] = {0};
     char temp[20]={0};
     // start polling
     if(minute(switchonTime) < 10 ) {
       sprintf(temp,"{\"srt\":\"0%d:0%d hr\"", hour(switchonTime), minute(switchonTime) );
     } else {
       sprintf(temp,"{\"srt\":\"0%d:%d hr\"", hour(switchonTime), minute(switchonTime) );
     }
     strcat(json, temp);
     if( minute(switchoffTime) < 10 ) {
        sprintf(temp,",\"sst\":\"%d:0%d hr\"}", hour(switchoffTime), minute(switchoffTime) );
     } else {
        sprintf(temp,",\"sst\":\"%d:%d hr\"}", hour(switchoffTime), minute(switchoffTime) );
     }
     strcat(json, temp);
     Serial.println("json length = " + String(strlen(json)));
     request->send(200, "text/json", json);
     //json = String();
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


// this link provides the inverterdata on the frontpage
server.on("/get.Power", HTTP_GET, [](AsyncWebServerRequest *request) {     
// set the array into a json object

uint8_t remote = 0;
if(checkRemote( request->client()->remoteIP().toString()) ) remote = 1; // for the menu link
char json[200]={0};
int i = atoi(request->arg("inv").c_str()) ;

      uint8_t night = 0; 
      if(!timeRetrieved ) { night=0; } else {
     //if(now() > switchonTime && now() < switchoffTime )
      if(dayTime) { night = 0; } else { night = 1; } 
      }
      snprintf(json, sizeof(json), "{\"nm\":\"%s\",\"polled\":\"%d\",\"type\":\"%d\",\"count\":\"%d\",\"remote\":\"%d\",\"state\":\"%d\",\"sleep\":\"%d\"" ,Inv_Prop[i].invLocation, polled[i], Inv_Prop[i].invType, inverterCount, remote, zigbeeUp, night );

      // now populate the values depending on if the panel exists and if polled
      char pan[4][50]={0};
      for(int z = 0; z < 4; z++ ) 
      {
         //is the panel connected? if not put n/e
           if( ! Inv_Prop[i].conPanels[z] ) { sprintf(pan[z], ",\"p%d\":\"n/e\"", z);  }  
         // so the panel is connected, is the inverter polled?
         else if (polled[i]) 
         {
             //polled, we put a value
               sprintf( pan[z], ",\"p%d\":\"%s\"", z, Inv_Data[i].power[z] );
         }   else {
            // not polled, we put n/a
               sprintf(pan[z], ",\"p%d\":\"n/a\"", z);
         }
      // add this to json
      strcat(json, pan[z]);
      }
     char tail[20]={0};
     if (polled[i]) {
       sprintf(tail, ",\"eN\":\"%.2f\"}", Inv_Data[i].en_total/(float)1000);
       strcat(json, tail);
    } else {
       strcat(json, ",\"eN\":\"n/a\"}");
    }

     request->send(200, "text/json", json);

 
});

// this link is called by the detailspage
server.on("/get.Inverter", HTTP_GET, [](AsyncWebServerRequest *request) { 
// this is used by the detailspage and for http requests      
// set the array into a json object

//  int panelCount=4;
  int i;
  if (request->hasArg("inv")) {
     i = (request->arg("inv").toInt()) ;
  } else {
     i = iKeuze;
  //request->send(200, "text/plain", "no argument provided");
  }
  //Serial.println("i = " + String(i));
  if( i < inverterCount) {

char json[350]={0} ;     
// we need name polled type id serial
snprintf(json, sizeof(json), "{\"inv\":\"%d\",\"name\":\"%s\",\"polled\":\"%d\",\"type\":\"%d\",\"serial\":\"%s\",\"sid\":\"%s\",\"freq\":%s,\"temp\":%s,\"acv\":%s,\"sq\":%s" , i, Inv_Prop[i].invLocation, polled[i], Inv_Prop[i].invType, Inv_Prop[i].invSerial, Inv_Prop[i].invID, Inv_Data[i].freq, Inv_Data[i].heath, Inv_Data[i].acv, Inv_Data[i].sigQ );

      
//// we need to provide values for all panel so when connected this is n/e
      char pan[4][50]={0};
      for(int z = 0; z < 4; z++ ) 
     {
         if(Inv_Prop[i].conPanels[z]) // is this panel connected?
         {
              sprintf(pan[z], ",\"dcv%d\":%s,\"dcc%d\":%s,\"pow%d\":%s,\"en%d\":%s" , z , Inv_Data[i].dcv[z], z ,Inv_Data[i].dcc[z],z ,Inv_Data[i].power[z], z, en_saved[i][z]);
         }   else {
              sprintf(pan[z], ",\"dcv%d\":\"n/e\",\"dcc%d\":\"n/e\",\"pow%d\":\"n/e\",\"en%d\":\"n/e\"", z ,z ,z ,z);
         }
         strcat(json, pan[z]);
      }
//      
    char tail[40]={0};   
    sprintf(tail, ",\"power\":%s,\"eN\":%.2f}", Inv_Data[i].power[4], Inv_Data[i].en_total/(float)1000);
    strcat(json, tail);

     request->send(200, "text/json", json);
    // json = String();
     } else {
     String term = "unknown inverter " + String(i);
     request->send(200, "text/plain", term);
     }
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
