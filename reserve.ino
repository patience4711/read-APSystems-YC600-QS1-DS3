//
//const char INVCONFIG_START[] PROGMEM = R"=====(
//<style>
//.cap {
//  font-weight:bold; 
//  Background-color:lightgreen;
// }
//
//div.overlay {
//  display: block;
//  width: 100%;
//  height: 100%;
//  background-color: rgba(0,0,0,0.7);
//  z-index: 0;
//  text-align: center;
//  vertical-align: middle;
//  line-height: 300px;
//}
//</style>
//<body onload='loadScript()'>
//<div id='msect'>
//  <div id='help'>
//  <span class='close' onclick='sl();'>&times;</span><h3>INVERTER SETTINGS HELP</h3>
//  <b>you can add max 9 inverters.</b><br><br>
//  <b>name / location:</b><br>
//  Enter a meaningfull name or the position of the inverter.
//  <br><br><b>inverter id:</b><br>
//  Enter the serialnr and the type of your inverter.<br>
//  Click on "save data" to add the inverter.<br>
//  Enter "000000000000" (12*0) to remove the inverter.
//  <br><br><b>pair:</b><br>
//  link your inverter to this ECU in order to make the zigbee communication possible.<br>
//  When paired you will see the obtained inverter ID in the status colomn.
//  You can find more information in the log or information page. 
//  <br><br>
//  </div>
//</div>
//
//<center>
//
//<div id='msect'>
//<ul>
//<li><a href='/MENU'>done</a></li>
//<li><a href='/INV?welke=0' style='display:none'0>inv. 0</a></li>
//<li><a href='/INV?welke=1' style='display:none'1>inv. 1</a></li>
//<li><a href='/INV?welke=2' style='display:none'2>inv. 2</a></li>
//<li><a href='/INV?welke=3' style='display:none'3>inv. 3</a></li>
//<li><a href='/INV?welke=4' style='display:none'4>inv. 4</a></li>
//<li><a href='/INV?welke=5' style='display:none'5>inv. 5</a></li>
//<li><a href='/INV?welke=6' style='display:none'6>inv. 6</a></li>
//<li><a href='/INV?welke=7' style='display:none'7>inv. 7</a></li>
//<li><a href='/INV?welke=8' style='display:none'8>inv. 8</a></li>
//</ul>
//</div>
//
//<div id='msect'>
//<div id='bo'></div>
//  <center><div class='divstijl'>
//<form id='formulier' method='get' action='inverterconfig' oninput='showSubmit()' onsubmit="return confirm('Are you sure you want to save this inverter?')">
//  
//    <irame name>
//    <br>
//  </div>
//</div>
//<div id='msect'>
//  <ul>
//  <div id='pairknop' style='display:none;'>
//    <li style='background:green;'><a href='/PAIR' onclick="return confirm('Are you sure you want to pair this inverter?')">pair this inverter</a></li>
//  </div>
//  <li><a href='#' onclick='helpfunctie()'>help</a></li>
//  <div id="submitknop" style='display:none;'>
//    <li><button class='sub' onclick='submitFunction("/SW=BACK")'>save data</button></li>
//  </div>
//</div>  
//</ul>
//<br>
//  
//</div>
//<script>
//
//function loadScript() {
//  getData();
//  loadData();
//}
//
//setInterval(function loadData() {
// getData()
//},30000);
//
//function getData() {
//  var xhttp = new XMLHttpRequest();
//  xhttp.onreadystatechange = function() {
//    if (this.readyState == 4 && this.status == 200) {
//      var antwoord = this.responseText;
//      var obj = JSON.parse(antwoord);
//      var polled = obj.polled;
//      var sn = obj.sn;
//      if(!sn){sn="000000000000";}
//      document.getElementById("iv").value = sn;
//      if(polled == "1") {
//        document.getElementById("veld").style.color = "green";
//        document.getElementById("veld").innerHTML="Inverter output available!";
//       }
//    }
//  };
//  xhttp.open("GET", "get.Power?which=99", true);
//  xhttp.send();
//}
//
//</script>
//
//</body></html>
// )=====";
//
// const char INVERTER_GENERAL[] PROGMEM = R"=====(
//<div id='inverter0' style='display:block'>
//<table><tr><td style='width:160px;'><h4>INVERTER {nr}</h4>
//<td style='width:70px;'><h4>STATUS:</h4><td>
//<input style='width:100px;' class='inp3' value='unpaired' readonly></tr></table>
//
//<h4 style='color:red' id='veld'>no output available from this inverter!</h4>
//    
//<br>
//<table style="background-color: lightgreen;">
//
//<tr><td class="cap" style="width:160px;">INVERTER SERIALN<td><input class='inp4' id='iv' name='iv' minlength='12' maxlength='12' required value='408000066782'></input>
//<tr><td class="cap">TYPE<td><select name='invt' class='sb1' id='sel'>
//<option value='0' selected>YC600</option><option value='1' invtype_1>QS1</option></select>
//</tr>
//<tr><td class="cap" >NAME / LOCATION<td class="cap" ><input class='inp4' id='il' name='il' maxlength='12' value='location'></input>
//<tr><td class="cap" >DOMOTICZ IDX<td class="cap" ><input class='inp2' name='mqidx' value='{idx}' size='4' length='4'></td></tr>
//</table></form>
//)=====";
////<br><div id='submknop' style='display:none'><button class='submbtn' type='submit'>save</button></form></div>
//void zendPageInverters() {
//DebugPrintln("we are now at zendPageInverters");
////toSend = FPSTR(INVCONFIG_START);
//// initieel wordt inverter 0 geopend als de pagina wordt geladen
//iKeuze=0;
//zendPageRelevantInverters();
//}
//
//// ****************************************************************************
//// we now show a page with for this inverter relevantinputs
//// ****************************************************************************
//void zendPageRelevantInverters() {
//  toSend = FPSTR(HTML_HEAD);
//  toSend += FPSTR(INVCONFIG_START);
//
//// can we adapt the visible menu items to reflect the count
//for(int i=0; i<inverterCount; i++) {
//toSend.replace("none\'" + String(i), "block\'") ;  
//}
//// now we see the present inverters, we show one more for add
////<li><a href='/INV?welke=8' style='display:none'8>inv. 8</a></li>
//if(inverterCount < 9) {
//String toRep = "none\'" + String(inverterCount) + ">inv. " + String(inverterCount);
////Serial.println("toRep = " + toRep); 
////toSend.replace("none\'" + String(invCount) + ">inv. " + String(invCount), "block"
//toSend.replace(toRep, "block\'>add");
//}
//
//// **********************************************************************
////        put back the inverterpage with actual data
//// **********************************************************************
//    toSend.replace("<irame name>" , FPSTR(INVERTER_GENERAL));  
//    toSend.replace("{nr}" , String(iKeuze)); // vervang inverter nummer not available
//    if(String(Inv_Prop[iKeuze].invSerial) != "000000000000") {
//    //toSend.replace("N/A", String(Inv_Prop[iKeuze].invSerial)); // handled by the script
//    toSend.replace("location", String(Inv_Prop[iKeuze].invLocation));
//    toSend.replace("{idx}", String(Inv_Prop[iKeuze].invIdx));
//    } else {
//    // if we called the add page
//    toSend.replace("location", "");
//    toSend.replace("{idx}", "0");      
//    }
//    // de selectbox
//    if(Inv_Prop[iKeuze].invType == 1 ) {
//    //Serial.println(" inverter type = 1");  
//    toSend.replace("invtype_1", "selected"); 
//    } else { 
//      //Serial.println(" inverter type = 0");
//      toSend.replace("invtype_0", "selected");    
//    }
//    if(String(Inv_Prop[iKeuze].invID) != "0x0000") {
//    toSend.replace("unpaired", String(Inv_Prop[iKeuze].invID) );
//    }
//    // make the pairbutton visible when there is a serial nr
//    if(String(Inv_Prop[iKeuze].invSerial) != "000000000000") {
//      toSend.replace("id='pairknop' style='display:none;'", "id='pairknop' style='display:block;'");
//    }
//  // now we have the page ready to send
//}
//
//
//
////*******************************************************************************************
////             prepare for saving the data
//// *****************************************************************************************
//void handleInverterconfig(AsyncWebServerRequest *request) { // form action = handleInverterconfig
//// we only collect the data for this specific inverter
//// read the serverargs and copy the values into the variables
//   DebugPrintln("we are in handleInverterconfig");    
//   strcpy(Inv_Prop[iKeuze].invLocation, request->arg("il").c_str());
//   strcpy(Inv_Prop[iKeuze].invSerial, request->arg("iv").c_str());
//
//   Inv_Prop[iKeuze].invType = request->arg("invt").toInt(); //values are 0 1   
//   Inv_Prop[iKeuze].invIdx = request->arg("mqidx").toInt(); //values are 0 1  
//   String bestand = "/Inv_Prop" + String(iKeuze) + ".str"; // /Inv_Prop0.str
//   writeStruct(bestand, iKeuze); // alles opslaan in SPIFFS 
// 
//   // oke now what if we saved a 000000000000 serial, we have to reorganaize
//   if( String(Inv_Prop[iKeuze].invSerial) == "000000000000" ) {
//   DebugPrintln("we saved a nill so we reorganize the files");
//   reorganize(); 
//   }
//   //request->send_P(200, "text/html", CONFIRM_INV); //send the html code to the client
//   actionFlag=10; // triggers the reboot
//      
//    }
//
//
//void reorganize() {
//  // this function seeks for an empty "000000000000" inverter and removes it
//  //inv 0 had 408000066782
//  //inv1 had 408000119348
//  //Serial.println("reorganize: inverterCount at start = " + String(inverterCount));
//  // check if there is a nill
//  for (int z=0; z<inverterCount; z++) { // we do this inverterCount times
//    if(String(Inv_Prop[z].invSerial) == "000000000000") {
//      //Serial.println("found an emty inverter nr " + String(z) + ", going to shift all one place up");
//      for(int i=z; i<9; i++) {
//       strcpy(Inv_Prop[i].invSerial, Inv_Prop[i+1].invSerial);
//       strcpy(Inv_Prop[i].invID, Inv_Prop[i+1].invID);
//       Inv_Prop[i].invType = Inv_Prop[i+1].invType;
//     }
//     // now the last one should become the emty one so we make this 0000000000
//     inverterCount--;
//     //Serial.println("reorganize: inverterCount at end = " + String(inverterCount));
//     strcpy(Inv_Prop[inverterCount].invSerial, "000000000000");
//    }
//  }
////when done we save this
// for (int i=0; i< inverterCount; i++) {
// String bestand = "/Inv_Prop" + String(i) + ".str"; // /Inv_Prop0.str
// writeStruct(bestand, i); // alles opslaan in SPIFFS
// }
//// now we remove all empty files
//  for (int z=0; z<9; z++) {
//       if(String(Inv_Prop[z].invSerial) == "000000000000") {
//          String bestand = "/Inv_Prop" + String(z) + ".str";
//          //Serial.println("file to remove = " + bestand);
//          if (LittleFS.exists(bestand)) {
//             LittleFS.remove(bestand);
//             //Serial.println("removed file " + bestand);
//          }
//       }
//   }
//
//   actionFlag=10; // zorgt voor de reboot
//}
