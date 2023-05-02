//var interval = 60000;
//setInterval(function loadTime() {
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
//  interval=60000;
//},interval);

const char INFOPAGE [] PROGMEM = R"=====(
<!DOCTYPE html><html><head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta charset='utf-8'>
<link rel="icon" type="image/x-icon" href="/favicon.ico" />
<title>ESP-ECU</title>
<style>
table, th, td {border: 1px solid blue; width:font-size:12px;} 
body {margin:10px 10px 0px 20px; font-family:'lato',Verdana,Sans-serif;font-size:12px;}
</style>
<script>
//var interval = 60000;

//setInterval(function loadData() {
//loadTime()();
//},15000);

function loadTime() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;
      var obj = JSON.parse(antwoord);
      var hr = obj.uur;
      var mn = obj.min;
      var tijd= hr + ":" + mn;
      document.getElementById('tijdveld').innerHTML=tijd;
      }
  };
  xhttp.open("GET", "get.currentTime", true);
  xhttp.send();
}
</script>
</head><body onload='loadTime()'><center>
)=====";

void handleInfo(AsyncWebServerRequest *request) {
//DebugPrintln("we zijn op handleInfopage() ");

toSend = FPSTR(INFOPAGE);
//toSend.replace("tieTel", swName );

//DebugPrintln("infopage in de string gezet) ");
toSend += F("<h2>ESP-ECU SYSTEM DATA</h2>");


toSend += F("<div style='position:fixed; text-align:center; width:100%;'><br><a href='/'><button style='width:150px; height:50px;text-align:center; font-weight:bold; font-size:20px; background:#db97b9;'>close</button></a></div>"); 

String zt = F("summertime");
switch (dst) {
  case 2: zt=F("wintertime"); break;
  case 0: zt=F("no dst set"); break;
}

toSend += F("<br><br><br><br>system time = <span style='font-size:20px;font-weight:bold' id='tijdveld'></span> hr.&nbsp&nbsp ");
toSend += zt + F("<br>");

toSend += F("firmware version : "); toSend += String(VERSION) + F("<br>");

toSend += F("time retrieved today : "); if ( timeRetrieved ) { toSend += F("yes<br>"); } else { toSend += F("no<br>"); }


long rssi = WiFi.RSSI();
toSend += F("wifi signalstrength = "); toSend += String(rssi) + F("<br>");

if ( Mqtt_Format != 0 ) { //bool == y en er is een mqtt adres, ja kijk dan of er een sensor is ingesteld
// check if connected
    String clientId = String( ESP.getChipId() );
    toSend += F("de mqtt clientId = : "); toSend += clientId + F("<br>");    
    if ( MQTT_Client.connected() ) {
       toSend += F("status mqtt : connected to "); toSend += Mqtt_Broker + F("<br>");
       } else {
       toSend += F("status mqtt : not connected<br>");
       }
    } else {
   toSend += F("mosquitto not configured<br>");
   toSend += F("check the mosquitto settings<br>");  
}

  int minutens = millis()/60000;
  int urens = minutens/60;
  int dagen = urens/24;
  toSend += F("system up time: "); toSend += String(dagen) + F(" days "); toSend += String(urens-dagen*24) + F(" hrs ");
  toSend += String(minutens - urens*60) + F(" min.<br> ");
  toSend += F("current errorCode = "); toSend += String(errorCode) + F("<br>"); 


toSend += F("<br><br><table><tr><TH> ESP INFORMATION</th></tr>");
toSend += F("<tr><td>ESP CHIP ID nr: <td>"); toSend += String(ESP.getChipId());
toSend += F("<td>Flash Chip ID nr: <td>"); toSend += String(ESP.getFlashChipId()) + F("</tr>");
toSend += F("<tr><td>IDE Flash size: <td>"); toSend += String(ESP.getFlashChipSize()) + F(" bytes");
//toSend += F("<td>Real Flash size<td>"); toSend += String(ESP.getFlashChipRealSize()) + F(" bytes</tr>");
toSend += F("<td>stack size<td>"); toSend += String( stack_size() ) + F(" bytes</tr>");
toSend += F("<tr><td>Free heap<td>"); toSend += String(ESP.getFreeHeap()) + F(" bytes<td>remote IP<td>");
toSend += request->client()->remoteIP().toString() + F("</table>");

toSend += F("<h4>variables dump</h4>");
toSend += "value=" + String(value)  + "  inverterCount=" + String(inverterCount) + "  zigbeeUp=" + String(zigbeeUp) + "<br>";
toSend += "switchonTime=" + String(switchonTime)  + "  switchoffTime=" + String(switchoffTime)+ "<br>";
toSend += "polled = " + String(polled[0]) + String(polled[1]) + String(polled[2]) + String(polled[3]) + 
String(polled[4]) + String(polled[5]) + String(polled[6]) + String(polled[7]) + String(polled[8]) + "  diagNose = " + String(diagNose) + "<br>";
toSend += "resetCounter = " + String(resetCounter);
toSend += "  pollOffset = " + String(pollOffset) + "  Mqtt_Format = " + String(Mqtt_Format) + "<br>";

#ifdef TEST
toSend += "  testCounter = " + String(testCounter) + " inv0 type = " + String(Inv_Prop[0].invType);
#endif

toSend += F("<h3>Content filesystem :</h3>");

Dir dir = LittleFS.openDir("/");
while (dir.next()) {
    toSend += String(dir.fileName()) + F("   size ");
    File f = dir.openFile("r");
    toSend += String(f.size()) + F("<br>");
}

 //DebugPrintln("end infopage "); 
 //DebugPrint("de lengte van toSend na de infopage = "); //DebugPrintln( toSend.length() );
 request->send(200, "text/html", toSend); //send the html code to the client
}

uint32_t stack_size() {
    char stack;
    return (uint32_t)stack_start - (uint32_t)&stack;
}
