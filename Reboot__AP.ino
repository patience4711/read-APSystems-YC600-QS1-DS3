
// void zendPageReboot() {
//  // als niet ingelogd als admin dan terug
//    if(!server.authenticate("admin", pswd)) { return server.requestAuthentication(); }
//      
//  toSend = FPSTR(HTML_HEAD);
//  toSend += FPSTR(CONFIRM);
//
//  server.send(200, "text/html", toSend); //send the html code to the client
//  delay(500);//wait half a second after sending the dataconfigSave();
//  DebugPrintln("basisconfig saved");
//  ESP.restart();
//}

//void startAP() {
//  // als niet ingelogd als admin dan terug
//    if(!server.authenticate("admin", pswd)) { return server.requestAuthentication(); }
//        DebugPrintln("Erasing the wifisettings");
//    String teZenden = F("<!DOCTYPE html><html><head><script type='text/javascript'>setTimeout(function(){ window.location.href='/SW=BACK'; }, 3000 ); </script>");
//    teZenden += F("</head><body><h2>OK the accesspoint is started.</h2>Wait until the led lights up.<br><br>then go to wifi-settings on your device and connect to ESP-");
//    teZenden += String(ESP.getChipId()) + " !";
//     server.send ( 200, "text/html", teZenden ); //zend bevestiging
//        delay(500);//wait half a second after sending the data 
//       delay(1);
//
//   
//    WiFi.disconnect(true);
//    delay(1000); // ruim de tijd om de knop los te laten anders komt ie in programmeermode
//
//    
//    ESP.restart();
//}

void loginAdmin(AsyncWebServerRequest *request) {
  String authFailResponse = "<h2>login failed <a href='/'>click here</a></h2>";
  const char* www_realm = "login as administrator."; 
    if (!request->authenticate("admin", pswd)) return request->requestAuthentication();
}
