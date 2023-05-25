class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    handlePortalRoot(request);
  }
};

void start_portal() {
//  swap_to_Usb();
  IPAddress apIP(192, 168, 4, 1);
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);
  
  String apNaam = "ECU-" + String(ESP.getChipId());
  String apPasswd = "123456789";
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  WiFi.softAP(apNaam);
  digitalWrite(led_onb, LED_AAN);
  Serial.println(F("scan start"));
  scanWifi();
  
  server.on("/", handlePortalRoot);
  server.on("/home", handlePortalRoot);
  server.on("/fwlink", handlePortalRoot);
  server.on("/redirect", handlePortalRoot);
  server.on("/wifiForm", handleForm);
  server.on("/wifiCon", handleConnect); //in wifiform ther is a form with action wifiForm
  server.on("/wifiConfirm", handleConfirm); //in wifiform there is a form with action wifiForm 
  server.on("/wifiClose", handleClose);   
  server.on("/FS_ERASE", eraseFiles);
  server.on("/STATIC_ERASE", eraseStatic);
  server.onNotFound(portalNotFound);
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
    
  server.begin();


 
  // ***************************************************************************
  //              this is the infinitive  loop 
  //****************************************************************************
  Serial.println(F("entering portal loop"));
  laatsteMeting = millis(); //voor de time-out
  static unsigned long heartbeat = 0;
  while (millis() < laatsteMeting + 300*1000UL) { // 5 minuten== 300 30 == 30sec
  if ( millis() > heartbeat + 10*1000UL ) {
    heartbeat = millis(); // elke 10 sec een heartbeat
    //Serial.print("a ");
  }
  yield();
  if (tryConnectFlag) { // there are credentials provided
      wifiConnect();
  }
//  
//  if(Serial.available()) { // make the serial monitor work
//    handle_Serial();
//   }
   if (value == 11) break;
    //dnsServer.processNextRequest();

  } 
  // ************************ end while loop ******************************* 

  //we only are here after a timeout, or we clicked close      
//  if (Timed_Out == true) {
    Serial.println(F("portal timed out, resetting..."));
    delay(1000);
    ESP.restart();
//    } 
}


// ********************************************************************
//                         M A I N  F U N C T I O N S

// ********************************************************************
//                 de homepagina van het portal
// ********************************************************************
void handlePortalRoot(AsyncWebServerRequest *request) {
//  always as we are here, portalstart is updated, 
//  so when there is activity in the portal we won't time out
unsigned long laatsteMeting = millis(); // update portalstart

// This function displays the main page of the portal.
// depending on the status of event we display messages or buttons

//  if (captivePortal(request)) { // If captive portal redirect instead of displaying the page.
//    return;
//  }
//Serial.println("we are in handlePortalRoot, event = " + String(event));

toSend = FPSTR(PORTAL_HEAD);
toSend += FPSTR(PORTAL_HOMEPAGE);
//toSend.replace("{haha}" , "if (window.location.hostname != \"192.168.4.1\") {window.location.href = 'http://192.168.4.1'};");
toSend.replace("{ma}", String(WiFi.macAddress()) );
//
      // when event = 101 we adapt the page with new data
      if (event == 101) {  // the 2e time that we are here we are connected 101 or not 100
          toSend.replace("hadden", "hidden"); // verberg de configuratieknop
          toSend.replace("close' hidden", "close'"); // show the close button
          String page = "";
          page += F("<div class=\"msg\">");
          page += F("connected to network <strong> unset<a>");
          page.replace("unset", WiFi.SSID());
          page += F("</strong><br><br>");
          page += F("The IP address is <strong><a href=\"http://");
          page += WiFi.localIP().toString();
          page += F("\">");
          page += WiFi.localIP().toString();
          page += F("</a></strong><br><br>");
          page += F("<h3 style='color:#FF0000';>Note down the ip-address and click \"close\"!</h3>");
      toSend.replace("<strong>Currently not connected to a wifi network!</strong>" , page);
      // show the close
      toSend.replace("close' hidden", "close'");
      }
      if (event==100) {  // when not connected
          String page = "";
          page+=F("Your attempt to connect to the network has failed!<br><br>");
          page+=F("Try again, click on wifi configuration!"); 
          toSend.replace("Currently not connected to a  wifi network!" , page);
      }
      // hadden hudden hodden hedden
      // hide the de wipe button if wiped and connected
      if ( LittleFS.exists("/basisconfig.json") && event!=101 ) { //show if file is present
       toSend.replace("erase' hidden", "erase'");
      } else {
        //Serial.println(F("no basisconfig.json"));  
      }
       // we show this when ! 101 and static ip
      if( static_ip[0] != '\0' && static_ip[0] != '0' && event!=101) { //show if static is present
       toSend.replace("static' hidden", "static'"); 
      }
//        sendHeaders();
       request->send(200, "text/html", toSend); //send the html code to the client
 
       event = 0; //for the next round 
}

// *********************************************************************
//                  page for enter credentials
// *********************************************************************
void handleForm(AsyncWebServerRequest *request) {
   laatsteMeting = millis();
// the first time we come here the networks are scanned

  toSend = FPSTR(PORTAL_HEAD);
  toSend += FPSTR(PORTAL_WIFIFORM);
  toSend.replace("{pw}",  String(pswd) );
  toSend.replace("{sl}",  String(securityLevel) );

if (networksFound == 0) {
      toSend.replace("aplijst" ,"WiFi scan not found networks. Restart configuration portal to scan again.");
    } else {
      //Serial.println(F("make a list of found networks"));
      String lijst = makeList(networksFound);
      Serial.println(F("lijst = ")); Serial.println(lijst);
    toSend.replace("aplijst", lijst);  
    }

//  sendHeaders();
  request->send(200, "text/html", toSend); //send the html code to the client
//  delay(500); //wait half a second after sending the data
//  deze pagina heeft een form met action="wifiCon", verwijst naar wifiConnect
}

 void handleConnect(AsyncWebServerRequest *request) {
  Serial.println(F("/wifiCon requested"));

  // this function saves the password and takes care for connectin
  laatsteMeting = millis(); //om de timeout te verversen
 // char ssid[33] = ""; // declared global
 // char pass[40] = "";

//      grab the serverparameters
strcpy( ssid, request->getParam("s")->value().c_str() );
strcpy( pass, request->getParam("p")->value().c_str() );  
strcpy( pswd, request->getParam("pw")->value().c_str() );
securityLevel = request->arg("sl").toInt();

  wifiConfigsave(); // save the admin passwd
  //  wifiConnect();  // won't work, only after reboot we are connected 
  tryConnectFlag = true;  // this takes care for the connectattempts in the loop

Serial.println("send confirm page  "); 
//toSend = FPSTR(PORTAL_HEAD);
//toSend += FPSTR(PORTAL_CONFIRM); 
if(event == 100) {
toSend.replace("{text}", "connection has failed");
} else {
toSend.replace("{text}", "connection success");
}    
AsyncWebServerResponse *response = request->beginResponse(200, "text/html", toSend);
   response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
   response->addHeader("Pragma", "no-cache");
   response->addHeader("Expires", "-1");
}

void handleConfirm(AsyncWebServerRequest *request) {
  toSend = FPSTR(PORTAL_HEAD);
  toSend += FPSTR(PORTAL_CONFIRM);
  request->send(200, "text/html", toSend);
}

// **************************************************************************
//               C L O S I N G  T H E  P O R T A L 
// **************************************************************************

void handleClose(AsyncWebServerRequest *request) {
   laatsteMeting = millis();
  String toSend = F("<!DOCTYPE html><html><head>");
  toSend += F("<script type='text/javascript'>setTimeout(function(){ window.location.href='/'; }, 1000 ); </script>");
  toSend += F("</head><body><h2>The esp is going to restart.!</h2>Do not forget to note down the ip address!!<br>");
  toSend += F("<h2>The ip address is : ");
  toSend += WiFi.localIP().toString();
  toSend += F("</h2></body></html>");
  request->send(200, "text/html", toSend);
  value = 11; // set a flag for restart
}

void eraseFiles(AsyncWebServerRequest *request) {
  Serial.println("LittleFS wipe and format");
  String toSend = F("<!DOCTYPE html><html><head>");
  toSend += F("<script type='text/javascript'>setTimeout(function(){ window.location.href='/redirect'; }, 1000 ); </script>");
  toSend += F("</head><body><h2>erasing the filesystem, please wait... !</h2></body></html>");
  request->send ( 200, "text/html", toSend ); //zend bevestiging

     if (LittleFS.exists("/wifiConfig.conf")) { LittleFS.remove("/wificonfig.json"); } 
     if (LittleFS.exists("/colorConfig.json")) { LittleFS.remove("/colorConfig.json"); }
     if (LittleFS.exists("/basisconfig.json")) { LittleFS.remove("/basisconfig.json"); }
     if (LittleFS.exists("/timerconfig.json")) { LittleFS.remove("/timerconfig.json"); }
     LittleFS.format();
     Serial.println("Wiping LittleFS");
}

void eraseStatic(AsyncWebServerRequest *request) {
   //DebugPrintln("wipe static ip");
  String toSend = F("<!DOCTYPE html><html><head>");
  toSend += F("<script type='text/javascript'>setTimeout(function(){ window.location.href='/redirect'; }, 500 ); </script>");
  toSend += F("</head><body><h2>erasing ip settings, please wait... !</h2></body></html>");
  static_ip[0] = '\0';
  static_ip[1] = '\0';
  wifiConfigsave();  
  request->send ( 200, "text/html", toSend ); //zend bevestiging
}

// **************************************************************************************
//                  P O R T A L  H E L P F U N C T I O N S
// **************************************************************************************

//**********************************************************************
//      try to connect
// **********************************************************************
void wifiConnect() {
  // we are here because bool tryConnectFlag was true in the loop
      digitalWrite(led_onb, LED_UIT);
       tryConnectFlag=false;
       laatsteMeting = millis();

      //DebugPrintln("we are in wifiConnect");

      WiFi.mode(WIFI_AP_STA);
      delay(500);

      if (connectWifi() == 1) {
         Serial.println("\nyoupy, connected");
         ledblink(3, 500);
         event=101;
       } else {
         Serial.println("could not connect, try again");
         digitalWrite(led_onb, LED_AAN); // 
         event=100;
        } 
 }

// ************************************************************************
//          try to connect with new credentials
// ************************************************************************

int connectWifi() {  

  if (ssid != "") {
    //Serial.println(F("we have a new ssid, trying to connect to that"));
    //trying to fix connection in progress hanging
    ETS_UART_INTR_DISABLE();
    wifi_station_disconnect();
    ETS_UART_INTR_ENABLE();
    WiFi.begin(ssid, pass);

  } else { 
    // we don't have new ssid, so we see if there is one saved
    if (WiFi.SSID().length() > 0) {
      //Serial.println(F("we have saved credentials, use these"));
      //trying to fix connection in progress hanging
      ETS_UART_INTR_DISABLE();
      wifi_station_disconnect();
      ETS_UART_INTR_ENABLE();
      WiFi.begin();
    }
  }
  int connectAttempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
     delay(1000);
     Serial.print("\nwifi state = " + String(WiFi.status()));
     connectAttempts += 1;
     if (connectAttempts==10) {break;}
  }
   //Serial.println(F("\nwe are out of  the for=loop, event = " + String(event) ));

   if(connectAttempts < 10 ) {
      checkFixed(); // set static ip if configured
      return 1;
   } else {
     return 0;
   }
}

void portalNotFound(AsyncWebServerRequest *request) {
  String message="file not found";
  AsyncWebServerResponse *response = request->beginResponse(404,"text/plain",message);
  response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  response->addHeader("Pragma", "no-cache");
  response->addHeader("Expires", "-1");
  request->send (response );
}  

void scanWifi() {
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  //Serial.println(F("scan done"));
  if (n == 0) {
    Serial.println(F("no networks found"));
  } else {
    Serial.print(n);
    Serial.println(F(" networks found"));
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  networksFound = n;
  //return n; // # of found networks
  }

// ********************************************************
//      put the found networks in a string
// ********************************************************
String makeList(int aantal) { // aantal is the count of found networks

// FIRST sort
      int indices[aantal];
      for (int i = 0; i < aantal; i++) {
        indices[i] = i;
      }
      // RSSI SORT
      for (int i = 0; i < aantal; i++) {
        for (int j = i + 1; j < aantal; j++) {
          if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
            std::swap(indices[i], indices[j]);
          }
        }
      }   
   
  String lijst="";
  for (int i = 0; i < aantal; i++) {

        String item = FPSTR(PORTAL_LIST);
        String rssiQ;
        //int quality = (WiFi.RSSI(i));
        int quality = WiFi.RSSI(indices[i]);
        //Serial.print(F("quality = " + String(quality)));
        // -50 is groter dan -80
        if (quality < -99){ continue; } // skip als kleiner dan -65 bijv -66
//        item.replace("{v}", WiFi.SSID(i));
        item.replace("{v}", WiFi.SSID(indices[i]));        
        item.replace("{r}", String(quality));
        item.replace("{i}", "");
        lijst += item;
  }
return lijst;  
}
  
