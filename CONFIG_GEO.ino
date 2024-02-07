const char GEOCONFIG[] PROGMEM = R"=====(
<body>
<div id='msect'><div id='bo'></div>
<ul><li id='fright'><span class='close' onclick='cl();'>&times;</span></li>
<li id='sub'><a href='#' onclick='submitFunction("/SW=BACK")'>save</a></li>
</div>
<div id='msect'>
<br><kop>GEOGRAPHICAL SETTINGS</kop><br><br>
</div>
<div id='msect'>
  <div class='divstijl' style='height:52vh;'>
  <form id='formulier' method='get' action='geoconfig' oninput='showSubmit()'>
  <center><table>
  <tr><td style='width:140px;'>latitude &deg<td><input class='inp3' name='be' length='8' placeholder='latitude' value='{be}'></input></tr>
  <tr><td>longitude &deg<td><input  class='inp3' name='le' length='8' placeholder='longitude' value='{le}'></input></tr> 
  <tr><td>gmtOffset<td><input class='inp2' name='tz' length='6' placeholder='minutes relative to GMT' value='{tz}'></input></tr>
  <tr><td>dst y/n<td><input type='checkbox' style='width:30px; height:30px;' name='ts' #check></input></tr></table> 
  
  </div></div><br></form>
<br></body></html>
)=====";



void zendPageGEOconfig( AsyncWebServerRequest *request ) {

    String webPage = FPSTR(HTML_HEAD);
    webPage += FPSTR(GEOCONFIG);  
    
    // put back the data
    
    webPage.replace("{le}",  String(longi,3) );
    webPage.replace("{be}",  String(lati,3) );
    
    webPage.replace("{tz}",  String(gmtOffset) );
    
    if (zomerTijd) { 
    //  Serial.println("zomerTijd = true");
      webPage.replace("#check", "checked");
      } 
    
        request->send(200, "text/html", webPage);
        webPage="";
}

void handleGEOconfig(AsyncWebServerRequest *request) {
//char static_ip2[16] = "";

  //de serverargumenten verzamelen
//strcpy(lengte, request->getParam("le")->value().c_str());

longi = request->getParam("le")->value().toFloat();

//strcpy(breedte, request->getParam("be")->value().c_str());

lati = request->getParam("be")->value().toFloat();

strcpy(gmtOffset, request->getParam("tz")->value().c_str());


//BEWARE CHECKBOX
String dag = "";
if(request->hasParam("ts")) {
dag = request->getParam("ts")->value();  
}
   if (dag == "on") { zomerTijd = true; } else { zomerTijd = false;}
   
      wifiConfigsave();

      actionFlag=25; // recalculate with these settings 
} 
