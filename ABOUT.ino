//<link rel="icon" type="image/x-icon" href="/favicon.ico" />
//
const char ABOUT [] PROGMEM = R"=====(
<!DOCTYPE html><html><head>
<meta http-equiv="refresh" content="180">
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta charset='utf-8'>
<link rel="stylesheet" type="text/css" href="/STYLESHEET_HOME">
<title>ESP-ECU</title>
<style>
table, th, td {border: 1px solid blue; width:font-size:12px; background-color: #ffffcc; }
th, td { padding-left: 4px; padding-right: 4px;}
th {background-color: #22ffb3;}
body {font-size:12px;} tr {height:26px;} 
</style>
<script type="text/javascript" src="SECURITY"></script>
<script> function cl() { window.location.href='/MENU'; }</script>
</head><body>
<div id='msect'><ul><li id='fright'><span class='close' onclick='cl();'>&times;</span></ul></div>
<center>
<h2>ESP-ECU SYSTEM DATA</h2>

)=====";

void handleAbout(AsyncWebServerRequest *request) {
  char page[1536] = {0};
  char temp[100]={0};
  strcpy_P(page, ABOUT);

  int minutens = millis()/60000;
  int urens = minutens/60;
  int dagen = urens/24;
 
  strcat(page, "<br><table><tr><TH colspan='2'> SYSTEM INFORMATION</th></tr>" );
  strcat(page, "<tr><td>firmware version<td>ESP-ECU-v10_8</tr>");
  if ( timeRetrieved ) strcat(page,"<tr><td>time retrieved<td>yes</tr>"); else strcat(page,"<tr><td>time retrieved<td>no</tr>");
  sprintf(temp, "<tr><td>systemtime<td> %d:%d " , hour(), minute());
  switch (dst) {
    case 1: strncat(temp, "summertime</td>", 19 ); break;
    case 2: strncat(temp, "wintertime</td>", 19 ); break;
    case 0: strncat(temp, "no dst set</td>", 19 ); 
  }
  strcat(page, temp);
  
  sprintf(temp, "<tr><td>system uptime<td>%d d %d h %d m </td>", dagen, urens-dagen*24, minutens-urens*60);
  strcat(page, temp);
  sprintf(temp, "<tr><td>wifi signalstrength<td>%lddB</td>", WiFi.RSSI());
  strcat(page, temp);
  sprintf(temp, "<tr><td>ESP CHIP ID nr: <td> %ld</td>", ESP.getChipId() );
  strcat(page, temp);
  sprintf(temp, "<tr><td>Free heap<td> %u bytes</td>", ESP.getFreeHeap() );
  strcat(page, temp);

  if ( Mqtt_Format != 0 ) { 

    // check if connected
    if ( MQTT_Client.connected() ) { //: add a dot
       sprintf(temp, "<tr><td>mqtt connected<td>%s</td>", Mqtt_Broker);
       } else {
       sprintf(temp, "<tr><td>mqtt status<td>not connected</td>");
       }
   } else {
       sprintf(temp, "<tr><td>mqtt status<td>not configured</td>");
   }
   strcat(page, temp);
  
  sprintf(temp, "<tr><td>zigbeeUp<td> %d</td>" ,zigbeeUp);
  strcat(page, temp);
//  sprintf(temp, "<tr><td>switchonTime<td>%ld<tr><td>switchoffTime<td> %d</tr>" , (long)switchonTime, (long)switchoffTime);
//  strcat(page, temp);
  sprintf(temp, "<tr><td>autopolling<td>%d<tr><td>polled<td> %d%d%d%d%d%d%d%d%d</tr>" , Polling, polled[0], polled[1], polled[2],polled[3], polled[4], polled[5], polled[6], polled[7], polled[8]);
  strcat(page, temp);
  sprintf(temp, "<tr><td>current errorCode<td>%d</td>", errorCode); 
  strcat(page, temp);
  sprintf(temp, "<tr><td>ZB resetCounter<td> %d</td></tr></table>" , resetCounter );
  strcat(page, temp);

 request->send(200, "text/html", page); //send the html code to the client
 memset(page, 0, sizeof(page));
}
