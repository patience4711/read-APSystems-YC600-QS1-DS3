const char BASISCONFIG[] PROGMEM = R"=====(

<body>
<div id=''>
  <div id='bo'></div>
  <div id='help'>
  <span class='close' onclick='sl();'>&times;</span><h3>SETTINGS HELP</h3>
  <b>ECU ID:</b><br>
  The id should have 12 characters e.g. D8A3011B9780
  <br><br><b>user passwd:</b><br>Grant others access to the ECU with  
  <span style='color:red;'>user</span> and the <span style='color:red;'>user passw.</span> 
  The user has no access to the settings.
  <br><br><b>poll offset</b><br>
  minutes relative to sunrise and sunset to stop/start polling.
  <br><br><b>auto polling</b><br>
  We can uncheck this to stop automatic polling. Now we can poll<br>
  via mqtt (e.g. {"POLL":0) or http (e.g. http://ip_of_ecu/POLL=0)
  <br><br>
  </div>
</div>
<div id='msect'>
    <ul>
    <li><a href='/'>home</a></li>
    </ul><br>
</div>

<div id='msect'>
<kop>ECU GENERAL SETTINGS</kop>
</div>

<div id='msect'>
  <div class='divstijl' style='width: 480px; height:56vh;'>
  <form id='formulier' method='get' action='basisconfig' oninput='showSubmit()'>
  <center><table>
  <tr><td style='width:140px;'>ecu id<td><input class='inp6' name='ecuid' value='{id}' minlength='12' maxlength='12' required></input><td></tr>
  <tr><td>user passwd<td><input  class='inp5' name='pw1' length='11' placeholder='max. 10 char' value='{pw1}' pattern='.{4,10}' title='between 4 en 10 characters'></input> 
  </td></tr>
  <tr><td style='width:140px;'>offset sun-set/rise<td><input class='inp2' type='number' min='-15' max='15' name='offs' value='{of}' size='4' ><td>
  <tr><td>auto polling<td><input type='checkbox' style='width:30px; height:30px;' name='pL' #check></input></td><tr>
  <td>DS3 conversion<td><input class='inp2' type='number' id='cali' name='cali' min='0.2' max='2' value='{ca}'</input></td><tr>

  </td></tr></table></form>
  </table>
  </div><br>
</div>
<div id='msect'>
  <ul>
  <li id='sub'><a href='#' onclick='submitFunction("/SW=BACK")'>save</a></li>
  <li><a href='/MENU'>done</a>
  <li><a href='#' onclick='helpfunctie()'>help</a>
  </ul>
</div></body></html>
)=====";

void zendPageBasis() {
    //DebugPrintln("we zijn nu op zendPageBasis");
    toSend = FPSTR(HTML_HEAD);
    toSend += FPSTR(BASISCONFIG);
    
    // replace data
    toSend.replace("'{id}'" , "'" + String(ECU_ID) + "'") ;
//    toSend.replace("'{pr}'" , "'" + String(pollRes) + "'") ;
    toSend.replace( "'{pw1}'" , "'" + String(userPwd) + "'") ;
    toSend.replace( "'{of}'" , "'" + String(pollOffset) + "'") ; 
    if (Polling) { 
      toSend.replace("#check", "checked");
    }
    toSend.replace( "'{ca}'" , "'" + String(calliBration) + "'") ;
}
void handleBasisconfig(AsyncWebServerRequest *request) { // form action = handleConfigsave
// verzamelen van de serverargumenten   
   strcpy(ECU_ID, request->arg("ecuid").c_str());
   strcpy(userPwd, request->arg("pw1").c_str());
//   pollRes = request->arg("pr").toInt();
//   hc_IDX = request->arg("hcidx").toInt();
   pollOffset = request->arg("offs").toInt();  
// this value gets currupted when it is negative, we get 256 -/- the number
// so -2 becomes 254
//   if (po > 200) { pollOffset = po - 256; } else { pollOffset = po; } 
   
   
   calliBration = request->arg("cali").toFloat();
//BEWARE CHECKBOX
String dag = "";
if(request->hasParam("pL")) {
dag = request->getParam("pL")->value();  
}
   if (dag == "on") { Polling = true; } else { Polling = false;}
  //toSend = FPSTR(CONFIRM);
  basisConfigsave();  // alles opslaan
 // request->send_P(200, "text/html", CONFIRM); //send the html code to the client
 
  //DebugPrintln("basisconfig saved");
  actionFlag=25; // recalculates the time with these new values 
}
