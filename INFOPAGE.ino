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
var interval = 100;
setInterval(function loadTime() {
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
  interval=2000;
},interval);
</script>
</head><body onload='loadTime()'>
)=====";

void handleInfo(AsyncWebServerRequest *request) {
//DebugPrintln("we zijn op handleInfopage() ");

toSend = FPSTR(INFOPAGE);
//toSend.replace("tieTel", swName );

//DebugPrintln("infopage in de string gezet) ");
toSend += "<h2>Hansiart ECU STATUS INFORMATION</h2>";

toSend += "<div style='position:fixed;'<br><a href='/MENU'><button style='width:150px; height:50px;text-align:center; font-weight:bold; font-size:20px; background:#db97b9;'>close</button></a></div>"; 

String zt = "summertime";
switch (dst) {
  case 2: zt="wintertime"; break;
  case 0: zt="no dst set"; break;
}

toSend += "<br><br><br><br>system time = <span style='font-size:20px;font-weight:bold' id='tijdveld'></span> hr.&nbsp&nbsp " + zt + "<br>";

toSend += "firmware version : " + String(VERSION) + "<br>";

toSend += "time retrieved today : "; if ( timeRetrieved ) { toSend += "yes<br>"; } else { toSend += "no<br>"; }


long rssi = WiFi.RSSI();
toSend += "the signalstrength of the connection to the router = " + String(rssi) + "<br>";

if ( Mqtt_Enabled ) { //bool == y en er is een mqtt adres, ja kijk dan of er een sensor is ingesteld
// check if connected
    //String clientId = "ESPClient#";
    String clientId = String( ESP.getChipId() );
    toSend += "de mqtt clientId = : " + clientId + "<br>";    
    if ( MQTT_Client.connected() ) {
    toSend += "status mqtt : connected to " + Mqtt_Broker + "<br>";
    } 
    } else {
   toSend += "mosquitto not configured<br>";
   toSend += "check the mosquitto settings<br>";  
}

  int minutens = millis()/60000;
  int urens = minutens/60;
  int dagen = urens/24;
  toSend += "system up time: " + String(dagen) + " days " + String(urens-dagen*24) + " hrs " + String(minutens - urens*60) + " min.<br> ";
  toSend += "current errorCode = " + String(errorCode) + "<br>"; 

String ssuur = String(hour(switchonTime));
String ssmin = String(minute(switchonTime));
if( minute(switchonTime) < 10 ) ssmin = "0" + ssmin;
ssuur += ":" + ssmin;
toSend += "polling starts at " + ssuur;

ssuur = String(hour(switchoffTime));
ssmin = String(minute(switchoffTime));
if( minute(switchoffTime) < 10 ) ssmin = "0" + ssmin;
ssuur += ":" + ssmin;
toSend += " and stops at " + ssuur + "<br>";

toSend += "<h4>PAIRED INVERTERS</h4>";
for(int x=0;x<inverterCount;x++) {
    if(String(Inv_Prop[x].invID) != "0x0000" ) {
      toSend += "inverter " + String(x) + " serialnr " + String(Inv_Prop[x].invSerial) + "  ID = " + String(Inv_Prop[x].invID) + "<br>"; 
    }
}

toSend += "<h4>INVERTER OUTPUT</h4>";
toSend += "<table><tr><td>inv#<td>ac volt<td>temp.<td>freq.<td>dc volt1<td>dc curr1<td>dc volt2<td>dc curr2<td>dc volt3<td>dc curr3<td>dc volt4<td>dc curr4"; 
for (int i=0; i < inverterCount; i++) {
toSend += "<tr><td>" + String(i);
if(polled[i]) {
        toSend += "<td>" + String(Inv_Data[i].acv) + "<td>" + String(Inv_Data[i].heath) + "<td>" + String(Inv_Data[i].freq);
        toSend += "<td>" + String(Inv_Data[i].dcv[0]) + "<td>" + String(Inv_Data[i].dcc[0]) + "<td>" + String(Inv_Data[i].dcv[1]) + "<td>" + String(Inv_Data[i].dcc[1]);
        
        if(Inv_Prop[i].invType == 2) { // add 2 channels extra
        toSend += "<td>" + String(Inv_Data[i].dcv[2]) + "<td>" + String(Inv_Data[i].dcc[2]) + "<td>" + String(Inv_Data[i].dcv[3]) + "<td>" + String(Inv_Data[i].dcc[3]);
        } else {
        toSend += "<td>n/a<td>n/a<td>n/a<td>n/a";
        }
  } else { // not polled
         toSend += "<td>n/a<td>n/a<td>n/a<td>n/a<td>n/a<td>n/a<td>n/a<td>n/a<td>n/a<td>n/a<td>n/a</tr>";
  }
}
toSend += "</table>";

toSend += "<br><br><table><tr><TH> ESP INFORMATION</th></tr>";
toSend += "<tr><td>ESP CHIP ID nr: <td>" + String(ESP.getChipId());
toSend += "<td>Flash Chip ID nr: <td>"  + String(ESP.getFlashChipId()) + "</tr>";
toSend += "<tr><td>IDE Flash size: <td>" + String(ESP.getFlashChipSize()) + " bytes";
toSend += "<td>Real Flash size<td>" +  String(ESP.getFlashChipRealSize()) + " bytes</tr>";
toSend += "<tr><td>Free memory<td>" +  String(ESP.getFreeHeap()) + " bytes<td>remote IP<td>" + request->client()->remoteIP().toString() + "</table>";

toSend += "<h4>variables dump</h4>";
toSend += "value=" + String(value)  + "  inverterCount=" + String(inverterCount) + "  zigbeeUp=" + String(zigbeeUp) + "<br>";
toSend += "pollRes= 300  now=" + String(now()) + "  switchonTime=" + String(switchonTime)  + "  switchoffTime=" + String(switchoffTime)+ "<br>";
toSend += "unixtime=" + String(now()) + "<br>";
toSend += "polled = " + String(polled[0]) + String(polled[1]) + String(polled[2]) + String(polled[3]) + String(polled[4]) + String(polled[5]) + String(polled[6]) + String(polled[7]) + String(polled[8]) + "<br>";
toSend += "resetCounter = " + String(resetCounter) + "  calliBration = " + String(calliBration, 2);

#ifdef TEST
toSend += "  testCounter = " + String(testCounter) + " inv0 type = " + String(Inv_Prop[0].invType);

#endif
toSend += "<h3>Content filesystem :</h3>";

Dir dir = LittleFS.openDir("/");
while (dir.next()) {
    toSend += String(dir.fileName()) + "   size ";
    File f = dir.openFile("r");
    toSend += String(f.size()) + "<br>";
}

 //DebugPrintln("end infopage "); 
 //DebugPrint("de lengte van toSend na de infopage = "); //DebugPrintln( toSend.length() );
 request->send(200, "text/html", toSend); //send the html code to the client
}
