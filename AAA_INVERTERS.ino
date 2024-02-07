const char INVCONFIG_START[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP-ECU</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" type="image/x-icon" href="/favicon.ico" />
<link rel="stylesheet" type="text/css" href="/STYLESHEET_HOME">
<link rel="stylesheet" type="text/css" href="/STYLESHEET_SUBS">
<script type='text/javascript'>

function showSubmit() {
document.getElementById("sub").style.display = "block";
}
function submitFunction(a) {
document.getElementById("bo").innerHTML="<br>wait...<br>saving<br>this<br>inverter"; 
document.getElementById("bo").style.display="block"; 
document.getElementById('formulier').submit();
setTimeout(function(){window.location.href=a;}, 3000 ); 
}
</script>
<style>
.cap {
  font-weight:bold; 
  Background-color:lightgreen;
 }
div.overlay {
  display: block;
  width: 100%;
  height: 100%;
  background-color: rgba(0,0,0,0.7);
  z-index: 0;
  text-align: center;
  vertical-align: middle;
  line-height: 300px;
}
</style>
<script>
function cl() {
window.location.href='/MENU';
}
</script>
</head>
<body onload='%LOADBAG%'>

<center>

<div id='msect'>
<ul>
<li id='fright'><span class='close' onclick='cl();'>&times;</span>
<li><a href='/INV?welke=0' style='display:%none'0%>inv. 0</a></li>
<li><a href='/INV?welke=1' style='display:%none'1%>inv. 1</a></li>
<li><a href='/INV?welke=2' style='display:%none'2%>inv. 2</a></li>
<li><a href='/INV?welke=3' style='display:%none'3%>inv. 3</a></li>
<li><a href='/INV?welke=4' style='display:%none'4%>inv. 4</a></li>
<li><a href='/INV?welke=5' style='display:%none'5%>inv. 5</a></li>
<li><a href='/INV?welke=6' style='display:%none'6%>inv. 6</a></li>
<li><a href='/INV?welke=7' style='display:%none'7%>inv. 7</a></li>
<li><a href='/INV?welke=8' style='display:%none'8%>inv. 8</a></li>
<li><a href='/INV?welke=99' style='display:%none'99%>add</a></li>
</ul>
</div>

<div id='msect'>
<div id='bo'></div>
  <center><div class='divstijl' style='height:64vh;'>
<form id='formulier' method='get' action='inverterconfig' oninput='showSubmit()' onsubmit="return confirm('sure to save this inverter?')">
  
    %<FORMPAGE>%
    <br>
  </div>
</div>
<div id='msect'>
  <form id='formular' method='get' action='/INV_DEL'></form>
  <ul>
  <div id='pairknop' style='display:%none'p% >
    <li ><a class='groen' href='/PAIR' onclick="return confirm('Are you sure you want to pair this inverter?')">pair</a></li>
    <li><a href='#' onclick='delFunction("/SW=BACK")'>delete</a></li>
  </div>
    <li><a href='#' onclick='helpfunctie()'>help</a></li>
    <li id='sub'><a href='#' onclick='submitFunction("/SW=BACK")'>save</a></li>
  
</div>  
</ul>
<br>
  
</div>
<script type="text/javascript" src="INVSCRIPT"></script>
</body></html>
 )=====";

 const char INVERTER_GENERAL[] PROGMEM = R"=====(
<div id='inverter0' style='display:block'>
    <table><tr><td style='width:160px;'><h4>INVERTER {nr}</h4>
    <td style='width:70px;'><h4>STATUS:</h4><td>
    <input style='width:100px;' class='inp3' value='unpaired' readonly></tr></table>
        
    <br>
    <table style="background-color: lightgreen; padding:10px;">
    
    <tr><td class="cap" style="width:100px;">SERIALNR<td><input class='inp4' id='iv' name='iv' minlength='12' maxlength='12' required value='000000'></input>
    <tr><td class="cap">TYPE<td><select name='invt' class='sb1' id='sel' onchange='myFunction()'>
    <option value='0' invtype_0>YC600</option>
    <option value='2' invtype_2>DS3</option>
    <option value='1' invtype_1>QS1</option></select>
    </tr>
    <tr><td class="cap" >NAME<td class="cap" ><input class='inp4' id='il' name='il' maxlength='12' value='{location}'></input>
    <tr><td class="cap" >DOM. IDX<td class="cap" ><input class='inp2' name='mqidx' value='{idx}' size='4' length='4'></td></tr>
    <tr><td class="cap" >PANELS: 
    <td style='width: 230px;'>
    1&nbsp;<input type='checkbox' name='pan1' #1check>
    2&nbsp;<input type='checkbox' name='pan2' #2check>
    <span id='invspan'>
    3&nbsp;<input type='checkbox' name='pan3' #3check>
    4&nbsp;<input type='checkbox' name='pan4' #4check></tr></td>
    </span>

</table></form>
)=====";

// **********************************************************************************
//                         script
// **********************************************************************************

const char INV_SCRIPT[] PROGMEM = R"=====(
function showFunction() {
  //alert("showFunction");
  document.getElementById("invspan").style.display = "inline";
}
function hideFunction() {
  //alert("showFunction");
  document.getElementById("invspan").style.display = "none";
}
function myFunction(){
 if(document.getElementById("sel").value == 1 ) { 
    showFunction();
 } else {
   hideFunction();
 }
}

function delFunction(a) {
  if(confirm("are you sure to delete this inverter ?")) {  
  document.getElementById("bo").innerHTML="<br>wait...<br>processing<br>your<br>request"; 
  document.getElementById("bo").style.display="block";
  document.getElementById('formular').submit();   
  setTimeout(function(){window.location.href=a;}, 3000 ); 
  }
}

)====="; 




//*******************************************************************************************
//             prepare for saving the data
// *****************************************************************************************
void handleInverterconfig(AsyncWebServerRequest *request) 
{ 
  // form action = handleInverterconfig
  // we only collect the data for this specific inverter
  // read the serverargs and copy the values into the variables

   //("we are in handleInverterconfig");    
 
   // collect the serverarguments
   strcpy(Inv_Prop[iKeuze].invLocation, request->arg("il").c_str());
   strcpy(Inv_Prop[iKeuze].invSerial, request->arg("iv").c_str());
   Inv_Prop[iKeuze].invType = request->arg("invt").toInt(); //values are 0 1 2  
   Inv_Prop[iKeuze].invIdx = request->arg("mqidx").toInt(); //values are 0 1  
// the selectboxes
   char tempChar[1] = "";
   String dag = request->arg("pan1");  // mqselect
   if ( dag == "on") { Inv_Prop[iKeuze].conPanels[0] = true; } else { Inv_Prop[iKeuze].conPanels[0] = false; }   
   dag = request->arg("pan2");  // mqselect
   if ( dag == "on") { Inv_Prop[iKeuze].conPanels[1] = true;} else { Inv_Prop[iKeuze].conPanels[1] = false; }

   Inv_Prop[iKeuze].conPanels[2] = false;
   Inv_Prop[iKeuze].conPanels[3] = false;
   //we only collect this when type = 1
   if(Inv_Prop[iKeuze].invType == 1) {
   dag = request->arg("pan3");  // mqselect
   if ( dag == "on") { Inv_Prop[iKeuze].conPanels[2] = true; }    
   dag = request->arg("pan4");  // mqselect
   if ( dag == "on") { Inv_Prop[iKeuze].conPanels[3] = true;}    
   }
   //Serial.println("checked panels are : " + String(Inv_Prop[iKeuze].conPanels[0])+ String(Inv_Prop[iKeuze].conPanels[2])+ String(Inv_Prop[iKeuze].conPanels[2])+ String(Inv_Prop[iKeuze].conPanels[3]));
   //is this a addition?
   String bestand = "/Inv_Prop" + String(iKeuze) + ".str"; // /Inv_Prop0.str
   //Serial.println("going to write " + bestand ); 
   //initial their both 0
   writeStruct(bestand, iKeuze); // alles opslaan in SPIFFS
   if(iKeuze == inverterCount) 
   {
    inverterCount += 1;
    //Serial.println("we appended, inverterCount now : " + String(inverterCount)); 
    }
   
   basisConfigsave();  // save inverterCount
   #ifdef DEBUG
   //("\ninverterCount after edit (saved) = " + String(inverterCount));  
   //("list of the files we have after edit");
   printInverters();
   #endif
       
//      #ifdef DEBUG    
//      Serial.println("files after the ops");
//      printInverters();            
//      #endif

    actionFlag=10; // triggers the reboot
}

//*******************************************************************************************
//             delete an inverter
// *****************************************************************************************
void handleInverterdel(AsyncWebServerRequest *request) 
{ 
  // form action = handleInverterconfig
  // we only collect the data for this specific inverter
  // read the serverargs and copy the values into the variables

   //("we are in handleInverterdel");    
   //Serial.println("inverterCount initial = " + String(inverterCount));
   //Serial.println("iKeuze = " + String(iKeuze));  

   String bestand = "Inv_Prop" + String(iKeuze) + ".str"; // /Inv_Prop0.str
   //Serial.println("remove file " + bestand ); 
 
   if(LittleFS.exists(bestand) ) LittleFS.remove(bestand);
   
   //Serial.println("list of the files we have after removed one");
   //printInverters();
   inverterCount -= 1;
   basisConfigsave();  // save inverterCount   
//   // now we may have a gap in the file order
//   // check if we have one and remove it
      remove_gaps();
      //Serial.println("list of the files after remove gaps");
    #ifdef DEBUG 
    printInverters(); 
    #endif
    //Serial.println("inverterCount after removal = " + String(inverterCount));

      actionFlag = 10;
}

//void printInverters() {      
//      Serial.println(F(" ****** excisting inverter files ******"));
//      for (int x=0; x < inverterCount+1; x++) 
//      {
//      String bestand = "/Inv_Prop" + String(x) + ".str";
//      
//      if(LittleFS.exists(bestand)) 
//          {
//              Serial.print(F("filename: ")); Serial.println(bestand);
//              printStruct(bestand);
//          }
//      }
//}

// say we have
// Inv_prop0.str
// Inv_prop1.str

// Inv_prop3.str
// Inv_Prop4.str

// after we found 3 and not 2 we have
// Inv_prop0.str
// Inv_prop1.str
// inv_prop2.str
// Inv_prop3.str we renamed this one so the gap moved
// Inv_Prop4.str

// remove the gaps
void remove_gaps() {
String bestand_1;
String bestand_2;
bool found = false;  

  for(int i=0; i < 10; i++ ) 
  {
      bestand_1 = "Inv_Prop" + String(i) + ".str";
      bestand_2 = "Inv_Prop" + String(i+1) + ".str";
      //Serial.println("bestand_1 = " + bestand_1);
      //Serial.println("bestand_2 = " + bestand_2);
      if(!LittleFS.exists(bestand_1) && LittleFS.exists(bestand_2)) 
      {
      //Serial.println(bestand_1 + " not exist and " + bestand_2 + " exists"); 
        found = true;
        LittleFS.rename(bestand_2, bestand_1); // file 2 becomes file 1
      //Serial.println("renamed " + bestand_1);
       // printInverters();    
      }
  }
  // we remove the last file
  if (found) 
    {
    bestand_1 = "Inv_Prop" + String(inverterCount) + ".str"; 
    if(!LittleFS.exists(bestand_1) ) LittleFS.remove(bestand_1);
    }
}
// ********************************************************************
//                     processor
// *********************************************************************
String processor(const String& var)
{
//
  if(var == "LOADBAG") 
  {
    //Serial.println("found LOADBAG");
    if(Inv_Prop[iKeuze].invType == 1) 
      {
      return F("showFunction()"); 
      } else {
      return F("hideFunction()");  
      }
  }
// make the menu items visable

  for(int x=0; x<9; x++) { // for every button we have to set the visibility
     String placeholder = "none'" + String(x);
     //Serial.println("placeholder = " + placeholder);
       if(var == "none'" + String(x) ) { 
        if (x < inverterCount) { return F("block'"); } else { return F("none'"); }
       }
  }
//   
   if(inverterCount < 9) {
    //Serial.println("show add button");
    if(var == "none'99") return F("block'"); // show the add button
   }
  
  if(var == "<FORMPAGE>") 
  {
  return(toSend);  
  }

if(var == "none'p") {
    String bestand = "Inv_Prop" + String(iKeuze) + ".str";
    if(LittleFS.exists(bestand)) 
    {
    // we make the pair and delete button visible
      return "block'";
    } else {
      return "none'";
    }
}

return String(); //return empty when no match
}


// we construct the form
void inverterForm() {
    //Serial.println("bool nix = " + String(nix));
int verklikker = 0;
    if (inverterCount >= 88 )
    { 
        verklikker = 88;
        inverterCount -= verklikker; // restore the original inverterCount
    }
    inverterCount += verklikker; // add 88 again
    // now we have 3 situations
    // inverterCount == 0, show the page currently no inverters
    // iKeuze < invertercount, we have an exixting inverter
    // iKeuze = invertercount, we add a new inverter
    // we clicked the add button then invertercount is at least 88
    if( inverterCount != 0 ) {
   
    // **********************************************************************
    //        construct the inverterpage with actual data
    // **********************************************************************
        if (inverterCount >= 88 ) inverterCount -= 88; // restore inverterCount
        toSend = FPSTR(INVERTER_GENERAL);  
        // is there a file iKeuze then
        String bestand = "Inv_Prop" + String(iKeuze) + ".str";
        if(LittleFS.exists(bestand)) 
       {
        //Serial.println("File exists" + bestand);
        //the file exists so we can display the values 
        toSend.replace("{nr}" , String(iKeuze)); // vervang inverter nummer not available
        toSend.replace("000000", String(Inv_Prop[iKeuze].invSerial)); // handled by the script
        toSend.replace("{location}", String(Inv_Prop[iKeuze].invLocation));
        toSend.replace("{idx}", String(Inv_Prop[iKeuze].invIdx));
        // the selectboxes
        if (Inv_Prop[iKeuze].conPanels[0]) { toSend.replace("#1check", "checked");}
        if (Inv_Prop[iKeuze].conPanels[1]) { toSend.replace("#2check", "checked");}
                
        if(Inv_Prop[iKeuze].invType != 1 ) { // when the type = yc600 (0) or ds3 (2)
              
            toSend.replace("onload='showFunction()", "onload='hideFunction()" );
            if(Inv_Prop[iKeuze].invType == 0) 
            { 
              toSend.replace("invtype_0", "selected");
            } else {
             toSend.replace("invtype_2", "selected");  
           }
        } else { // inv type == 1 
          
          //Serial.println(" inverter type = 1");
          toSend.replace("invtype_1", "selected");
           if (Inv_Prop[iKeuze].conPanels[2]) { toSend.replace("#3check", "checked");}
           if (Inv_Prop[iKeuze].conPanels[3]) { toSend.replace("#4check", "checked");}
        }
        
        if(String(Inv_Prop[iKeuze].invID) != "0000") 
        {
           toSend.replace("unpaired", String(Inv_Prop[iKeuze].invID) );
        }

        } else {
        // the file does not exist so we show an empty page
        //Serial.println("File does not exist");
        toSend.replace("invtype_2", "selected");
        toSend.replace("000000", "");
        toSend.replace("{location}", "");
        toSend.replace("{idx}", "0");
        }

    } else { // so if inverterCount == 0 we present this page
     toSend = "<br><br><br><h3>currently no inverters</h3>"; 
    }
// now we have toSend ready to include in the inverterpage
}
