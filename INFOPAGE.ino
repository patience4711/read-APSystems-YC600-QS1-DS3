//
//const char INFOPAGE [] PROGMEM = R"=====(
//<!DOCTYPE html><html><head>
//<meta name="viewport" content="width=device-width, initial-scale=1">
//<meta charset='utf-8'>
//<link rel="icon" type="image/x-icon" href="/favicon.ico" />
//<title>ESP-ECU</title>
//<style>
//table, th, td {border: 1px solid blue; width:font-size:12px;} 
//body {margin:10px 10px 0px 20px; font-family:'lato',Verdana,Sans-serif;font-size:12px;}
//</style>
//<script>
//
//function loadTime() {
//  var xhttp = new XMLHttpRequest();
//  xhttp.onreadystatechange = function() {
//    if (this.readyState == 4 && this.status == 200) {
//      var antwoord = this.responseText;
//      var obj = JSON.parse(antwoord);
//      var hr = obj.uur;
//      var mn = obj.min;
//      var tijd= hr + ":" + mn;
//      document.getElementById('tijdveld').innerHTML=tijd;
//      }
//  };
//  xhttp.open("GET", "get.currentTime", true);
//  xhttp.send();
//}
//</script>
//<script type="text/javascript" src="SECURITY"></script>
//</head><body onload='loadTime()'><center>
//)=====";
//
//void handleInfo(AsyncWebServerRequest *request) {
////DebugPrintln("we zijn op handleInfopage() ");
//String toSEND = "";
//toSEND = FPSTR(INFOPAGE);
////toSEND.replace("tieTel", swName );
//
////DebugPrintln("infopage in de string gezet) ");
//toSEND += F("<h2>ESP-ECU SYSTEM DATA</h2>");
//
//
//toSEND += F("<div style='position:fixed; text-align:center; width:100%;'><br><a href='/'><button style='width:150px; height:50px;text-align:center; font-weight:bold; font-size:20px; background:#db97b9;'>close</button></a></div>"); 
//
//String zt = F("summertime");
//switch (dst) {
//  case 2: zt=F("wintertime"); break;
//  case 0: zt=F("no dst set"); break;
//}
//
//toSEND += F("<br><br><br><br>system time = <span style='font-size:20px;font-weight:bold' id='tijdveld'></span> hr.&nbsp&nbsp ");
//toSEND += zt + F("<br>");
//
//toSEND += F("firmware version : "); toSEND += String(VERSION) + F("<br>");
//
//toSEND += F("time retrieved today : "); if ( timeRetrieved ) { toSEND += F("yes<br>"); } else { toSEND += F("no<br>"); }
//
//
//long rssi = WiFi.RSSI();
//toSEND += F("wifi signalstrength = "); toSEND += String(rssi) + F("<br>");
//
//if ( Mqtt_Format != 0 ) { //bool == y en er is een mqtt adres, ja kijk dan of er een sensor is ingesteld
//// check if connected
//    String clientId = String( ESP.getChipId() );
//    toSEND += F("de mqtt clientId = : "); toSEND += clientId + F("<br>");    
//    if ( MQTT_Client.connected() ) {
//       toSEND += F("status mqtt : connected to "); toSEND += Mqtt_Broker + F("<br>");
//       } else {
//       toSEND += F("status mqtt : not connected<br>");
//       }
//    } else {
//   toSEND += F("mosquitto not configured<br>");
//   toSEND += F("check the mosquitto settings<br>");  
//}
//
//  int minutens = millis()/60000;
//  int urens = minutens/60;
//  int dagen = urens/24;
//  toSEND += F("system up time: "); toSEND += String(dagen) + F(" days "); toSEND += String(urens-dagen*24) + F(" hrs ");
//  toSEND += String(minutens - urens*60) + F(" min.<br> ");
//  toSEND += F("current errorCode = "); toSEND += String(errorCode) + F("<br>"); 
//
//
//toSEND += F("<br><br><table><tr><TH> ESP INFORMATION</th></tr>");
//toSEND += F("<tr><td>ESP CHIP ID nr: <td>"); toSEND += String(ESP.getChipId());
//toSEND += F("<td>Flash Chip ID nr: <td>"); toSEND += String(ESP.getFlashChipId()) + F("</tr>");
//toSEND += F("<tr><td>IDE Flash size: <td>"); toSEND += String(ESP.getFlashChipSize()) + F(" bytes");
////toSEND += F("<td>Real Flash size<td>"); toSEND += String(ESP.getFlashChipRealSize()) + F(" bytes</tr>");
//toSEND += F("<td>stack size<td>"); toSEND += String( stack_size() ) + F(" bytes</tr>");
//toSEND += F("<tr><td>Free heap<td>"); toSEND += String(ESP.getFreeHeap()) + F(" bytes<td>remote IP<td>");
//toSEND += request->client()->remoteIP().toString() + F("</table>");
//
//toSEND += F("<h4>variables dump</h4>");
//toSEND += "value=" + String(value)  + "  inverterCount=" + String(inverterCount) + "  zigbeeUp=" + String(zigbeeUp) + "<br>";
//toSEND += "switchonTime=" + String(switchonTime)  + "  switchoffTime=" + String(switchoffTime)+ "<br>";
//toSEND += "autopolling=" + String(Polling) + "  polled = " + String(polled[0]) + String(polled[1]) + String(polled[2]) + String(polled[3]) + String(polled[4]) + String(polled[5]) + String(polled[6]) + String(polled[7]) + String(polled[8]) + "<br>";
//toSEND += "pollOffset = " + String(pollOffset) + "  Mqtt_Format = " + String(Mqtt_Format) + "<br>";
//toSEND += "Zigbee resetCounter = " + String(resetCounter) + "<br>";
//#ifdef TEST
//toSEND += "  testCounter = " + String(testCounter) + " inv0 type = " + String(Inv_Prop[0].invType);
//#endif
//
//toSEND += F("<h3>Content filesystem :</h3>");
//
//Dir dir = LittleFS.openDir("/");
//while (dir.next()) {
//    toSEND += String(dir.fileName()) + F("   size ");
//    File f = dir.openFile("r");
//    toSEND += String(f.size()) + F("<br>");
//}
//
// //DebugPrintln("end infopage "); 
// //DebugPrint("de lengte van toSEND na de infopage = "); //DebugPrintln( toSEND.length() );
// request->send(200, "text/html", toSEND); //send the html code to the client
//}
//
//uint32_t stack_size() {
//    char stack;
//    return (uint32_t)stack_start - (uint32_t)&stack;
//}
