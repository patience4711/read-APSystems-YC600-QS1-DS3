void start_asyserver() {

server.on("/details", HTTP_GET, [](AsyncWebServerRequest *request) {
iKeuze = atoi(request->arg("inv").c_str()) ; // so we know for which inverter
strcpy( requestUrl, request->url().c_str() );
request->send_P(200, "text/html", DETAILSPAGE);
});
//server.on("/DETAILSSCRIPT", HTTP_GET, [](AsyncWebServerRequest *request) {
//    request->send_P(200, "text/css", DETAILS_SCRIPT);
//});
server.on("/CONSOLE", HTTP_GET, [](AsyncWebServerRequest *request){
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    request->send_P(200, "text/html", CONSOLE_HTML);
  });

// Simple Firmware Update
  server.on("/FWUPDATE", HTTP_GET, [](AsyncWebServerRequest *request){
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );    
    strcpy( requestUrl, "/" );
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
      //("filename empty, aborting");
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
    request->redirect( String(requestUrl) );
});

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
//    loginBoth(request, "both");
    request->send_P(200, "text/html", ECU_HOMEPAGE );
//AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ECU_HOMEPAGE, sizeof(ECU_HOMEPAGE));
//response->addHeader("Server","ESP ECU SYSTEM");
//request->send(response);
});

server.on("/STYLESHEET_HOME", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", STYLESHEET_HOME);
});
server.on("/STYLESHEET_SUBS", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", STYLESHEET_SUBS);
});
//server.on("/JAVASCRIPT", HTTP_GET, [](AsyncWebServerRequest *request) {
//    request->send_P(200, "text/css", JAVA_SCRIPT);
//});
server.on("/SECURITY", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", SECURITY);
});
server.on("/INVSCRIPT", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", INV_SCRIPT);
});
//server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
//    //Serial.println("favicon requested");
//    AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", FAVICON, FAVICON_len);
//    request->send(response);
//});

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
    strcpy( requestUrl, request->url().c_str() );// remember this to come back after reboot
    zendPageBasis(request);
    //request->send(200, "text/html", toSend);
});

server.on("/basisconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleBasisconfig(request);
    //request->send(200, "text/html", toSend);
    request->redirect( String(requestUrl) );
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
    strcpy( requestUrl, request->url().c_str() );
    zendPageMQTTconfig(request);
});

server.on("/MQTTconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
handleMQTTconfig(request);
request->redirect( String(requestUrl) );
});

server.on("/GEOCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    strcpy( requestUrl, request->url().c_str() );
    zendPageGEOconfig(request);
});

server.on("/geoconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
    //DebugPrintln(F("geoconfig requested"));
    handleGEOconfig(request);
    request->redirect( String(requestUrl) );
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

server.on("/ABOUT", HTTP_GET, [](AsyncWebServerRequest *request) {
    //Serial.println(F("/INFOPAGE requested"));
    loginBoth(request, "both");
    handleAbout(request);
});
server.on("/TEST", HTTP_GET, [](AsyncWebServerRequest *request) {
     if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    loginBoth(request, "admin");
    actionFlag = 44;
    request->send( 200, "text/html", "<center><br><br><h3>checking zigbee.. please wait a minute.<br>Then you can find the result in the log.<br><br><a href=\'/PAGE\'>click here</a></h3>" );
});
#ifdef LOG
server.on("/LOGPAGE", HTTP_GET, [](AsyncWebServerRequest *request) {
    loginBoth(request, "both");
    strcpy( requestUrl, request->url().c_str() );
    //handleLogPage(request);
    request->send_P(200, "text/html", LOGPAGE, putList);
});

//server.on("/CLEAR_LOG", HTTP_GET, [](AsyncWebServerRequest *request) {
//      if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
//      loginBoth(request, "admin");
//      Clear_Log(request);
//      request->redirect( "/LOGPAGE" ); // refreshes the page
//});
#endif
 
// ********************************************************************
//                    inverters
// ******************************************************************
server.on("/PAIR", HTTP_GET, [](AsyncWebServerRequest *request) {
      if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
      loginBoth(request, "admin");
      strcpy( requestUrl, request->url().c_str() );
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
});

server.on("/INV", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    strcpy( requestUrl, "/");
    int i = atoi(request->arg("welke").c_str()) ;
    iKeuze = i;
    if( iKeuze == 99 ) {
      iKeuze = inverterCount; //indicate this is an addition
      inverterCount += 88;
    }
    String bestand = "/Inv_Prop" + String(iKeuze) + ".str";
    if (!LittleFS.exists(bestand)) Inv_Prop[iKeuze].invType = 2;
    inverterForm(); // prepare the form page
    request->send_P(200, "text/html", INVCONFIG_START, processor); //send the html code to the client
});

server.on("/CONFIRM_INV", HTTP_GET, [](AsyncWebServerRequest *request) {
    toSend = FPSTR(CONFIRM_INV); // prepare the page
    request->send(200, "text/html", toSend); //send the html code to the client
});

// Handle Web Server Events
events.onConnect([](AsyncEventSourceClient *client){
//  if(client->lastId()){
//    Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
//  }
});
server.addHandler(&events);



// ********************************************************************
//                    X H T  R E Q U E S T S
//***********************************************************************

server.on("/get.Paired", HTTP_GET, [](AsyncWebServerRequest *request) {     
// set the array into a json object
String json="{";
     json += "\"invID\":\"" + String(Inv_Prop[iKeuze].invID) + "\"";
     json += "}";
     request->send(200, "text/json", json);
     json = String();
});

server.on("/get.General", HTTP_GET, [](AsyncWebServerRequest *request) {
// this link provides the general data on the frontpage
    char temp[13]={0};
    uint8_t remote = 0;
    if(checkRemote( request->client()->remoteIP().toString()) ) remote = 1; // for the menu link
    uint8_t night = 0; 
    if(!dayTime) { night = 1; }

// {"ps":"05:27 hr","pe":"21:53 hr","cnt":3,"rm":0,"st":1,"sl":1}  length 62
    
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    //StaticJsonDocument<160> doc; //(160);
    DynamicJsonDocument root(160); //(160);
    //JsonObject root = doc.to<JsonObject>();
     if(minute(switchonTime) < 10 ) {
       sprintf(temp,"0%d:0%d hr", hour(switchonTime), minute(switchonTime) );
     } else {
       sprintf(temp,"0%d:%d hr", hour(switchonTime), minute(switchonTime) );
     }
     root["ps"] = String(temp);

     if( minute(switchoffTime) < 10 ) {
        sprintf(temp,"%d:0%d hr", hour(switchoffTime), minute(switchoffTime) );
     } else {
        sprintf(temp,"%d:%d hr", hour(switchoffTime), minute(switchoffTime) );
     }
    root["pe"] = String(temp);
    root["cnt"] = inverterCount;    
    root["rm"] = remote;
    root["st"] = zigbeeUp;
    root["sl"] = night;    
    serializeJson(root, * response);
    request->send(response);
});

server.on("/get.Power", HTTP_GET, [](AsyncWebServerRequest *request) {

    int i = atoi(request->arg("inv").c_str()) ;
    // {"eN":1.86,"p":[168.5,159.8,"200.6","300.3"]} length 53
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument root(128);
    // in energy_saved are the values stored
    if(Inv_Data[i].en_total > 0) { // only possible when was polled this day
        root["eN"] = round2(Inv_Data[i].en_total/(float)1000); // rounded
    } else {
        root["eN"] = "n/a" ;
    }    
    //now populate the powervalues in an array "p":[p0, p1, p2, p3]
    for(int z = 0; z < 4; z++ ) 
    {
         //is the panel connected? if not put n/e
         if( ! Inv_Prop[i].conPanels[z] ) { root["p"][z] = "n/e";  }  
         // so the panel is connected, is the inverter polled?
         else if (polled[i]) 
         {
             //polled, we put a value
               root["p"][z] = round1(Inv_Data[i].power[z]) ;
         }   else {
            // not polled, we put n/a
               root["p"][z] = "n/a";
         }
     }

    serializeJson(root, * response);

    request->send(response);

});

server.on("/get.Inverter", HTTP_GET, [](AsyncWebServerRequest *request) { 
// this is used by the detailspage and for http requests      
// set the array into a json object
//{"inv":0,"name":"BALKON","polled":true,"type":0,"serial":"408000158215","sid":"3A10","freq":50,"temp":36.6,"acv":227.6,"sq":51.4,"pw_total":372,"en_total":1.89,"dcv":[33.2,34.3,44.4,55,5],
//"dcc":[9.6,8.4,5.5,6.6],"pow":[189.6,182.5,189.6,182.5],"en":[967.86,918.52,967.86,918.52]} length 279
  
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  //StaticJsonDocument<768> doc; //(768);
  DynamicJsonDocument root(768); //);
  //JsonObject root = doc.to<JsonObject>();
  int i;
  if (request->hasArg("inv")) {
     i = (request->arg("inv").toInt()) ;
  } else {
     i = iKeuze;
  }
  if( i < inverterCount) {
    root["inv"] = i;
    root["name"] = Inv_Prop[i].invLocation;
    root["polled"] = polled[i];
    root["type"] = Inv_Prop[i].invType;
    root["serial"] = Inv_Prop[i].invSerial;
    root["sid"] = Inv_Prop[i].invID;
    
    root["freq"] = round1(Inv_Data[i].freq);
    root["temp"] = round1(Inv_Data[i].heath);
    root["acv"] = round1(Inv_Data[i].acv);
    root["sq"] = round1(Inv_Data[i].sigQ);     

    root["pw_total"] = round1(Inv_Data[i].pw_total);
    root["en_total"] = round2(Inv_Data[i].en_total/(float)1000); // rounded
     
      for(int z = 0; z < 4; z++ ) 
      {
         if(Inv_Prop[i].conPanels[z]) // is this panel connected?
         {
              root["dcv"][z] = round1(Inv_Data[i].dcv[z]);
              root["dcc"][z] = round1(Inv_Data[i].dcc[z]);
              root["pow"][z] = round1(Inv_Data[i].power[z]);
              root["en"][z] = round2(en_saved[i][z]); //rounded

         }   else {

              root["dcv"][z] = "n/e";
              root["dcc"][z] = "n/e";
              root["pow"][z] = "n/e";
              root["en"][z] = "n/e";

              //root["size"] = root.size();
         }

      }


    serializeJson(root, * response);
    request->send(response);

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

double round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}
double round1(double value) {
   return (int)(value * 10 + 0.5) / 10.0;
}
