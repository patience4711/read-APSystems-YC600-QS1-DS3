//<link rel="icon" type="image/x-icon" href="/favicon.ico" />

const char CONSOLE_HTML[] PROGMEM = R"=====(
<!DOCTYPE html><html><head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<title>ESP-ECU</title>
<meta name="viewport" content="width=device-width, initial-scale=1">

<link rel="stylesheet" type="text/css" href="/STYLESHEET_HOME">
<script>
function helpfunctie() {
document.getElementById("help").style.display = "block";
}
function sl() {  
document.getElementById("help").style.display = "none";
}

</script>

<style>
 tr {height:16px !important;
 font-size:15px !important;
 } 
 li a:hover {
   background-color: #333 !important;
}
#help {
  background-color: #ffffff; 
  border: solid 2px; 
  display:none; 
  padding:4px;
  width:94vw;
}
</style>
</head>
<body>
  <div id='help'>
  <span class='close' onclick='sl();'>&times;</span><h3>CONSOLE COMMANDS</h3>
  <b>10;ZBT=message: </b> send a zigbee message (e.g. 2710).<br><br>
  <b>10;DELETE=filename: </b> delete a file.<br><br>
  <b>10;INV_REBOOT: </b> reboot an unresponsive inverter<br><br>
  <b>10;HEALTH: </b> healthcheck zigbee hw/system<br><br>
  <b>10;POLL=x: </b> poll inverter #x<br><br>
  <b>10;INIT_N: </b> start the zigbee coordinator<br><br>
  <b>10;DIAG: </b> more Debug messages in console<br><br>
  <b>10;EDIT=0-AABB: </b> mark an inverter as paired<br><br>
  <b>10;ERASE: </b> delete all inverter files<br><br>
  <b>10;FILES: </b> show filesystem<br><br>
  <b>10;TESTMQTT: </b>sends a mqtt testmessage<br><br>  
  <b>10;CLEAR: </b> clear console window<br><br> 
  </div>

<div id='msect'>
<ul>
<li id='fright'><a href='/MENU' onclick='confirmExit()' class='close'>&times;</span></a>
<li><a href='#' onclick='helpfunctie()'>help</a>
<li><a><input type="text" placeholder="type here" id="tiep"></a>
</ul>
</div>  
<br>  
<div id='msect'>
  <div class='divstijl' style='height:84vh; border:1px solid; padding-left:10px;'>
  <table id='tekstveld'></table>
  </div>
 </div>

<script>
  var field = document.getElementById('tekstveld');
  var gateway = `${(window.location.protocol == "https:"?"wss":"ws")}://${window.location.hostname}/ws`;
  var websocket;
  var inputField = document.getElementById('tiep');

  window.onbeforeunload = confirmExit;
  function confirmExit()
  {
      alert("close the console?");
      ws.close();  
  }  
  
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
    field.insertAdjacentHTML('beforeend', "<tr><td>* * connection opened * *");
    inputField.focus();
    }
  function onClose(event) {
    console.log('Connection closed');
    field.insertAdjacentHTML('beforeend', "<tr><td>* * connection closed * *");
    //setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    //var message = event.data;
    field.insertAdjacentHTML('beforeend', "<tr><td>" + event.data );
    if (field.rows.length > 20) {
    var rtm = field.rows.length - 20;
    for (let x=0; x<rtm; x++) { field.deleteRow(0); }
  }
    if (event.data == "clearWindow") { 
    for (let i = 0; i < 22; i++) {
        field.deleteRow(0); }
    }
   }
 
  function onLoad(event) {
    initWebSocket();
    sendEvent();
  }

  function sendEvent() {
    inputField.addEventListener('keyup', function(happen) {
    if (happen.keyCode === 13) {
       happen.preventDefault();
       sendData();
       }   
    });
  }  
  function sendData(){
  var data = inputField.value; 
  websocket.send(data, 1);
  inputField.value = "";
  }

function disConnect() {
  alert("close the console");
  ws.close(); 
}
</script>
</body>
</html>
)=====";


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;

  for(int i=0; i<len; i++ ) 
  {
  txBuffer[i] = data[i];
  }
  txBuffer[len]='\0'; // terminate the array

  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
  {
      data[len] = 0;

     
           if (strncasecmp(txBuffer+3,"INV_REBOOT",10) == 0) {
              ws.textAll("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n<br>");
              ws.textAll("Reboot an inverter that stopped working.");
              ws.textAll("Characteristics: not responsive, (slow blinking red led).");
              ws.textAll("type REBOOT_INVERTER=x (x=inverternumber 0, 1 etc.)");
              ws.textAll("DISCLAIMER: THIS HAS NOT BEEN TESTED, USE AT YOUR OWN RISK!");
              return;
          } else         

          if (strncasecmp(txBuffer+3,"POLL=",5) == 0) {
            //input can be 10;POLL=0; 
            //ws.textAll("received " + String( (char*)data) + "<br>"); 
              int kz = String(txBuffer[8]).toInt();
              if ( kz > inverterCount-1 ) {
              ws.textAll("error, no such inverter");
              if ( kz == 9 ) actionFlag=48; // poll all
              return;  
              }
              ws.textAll("poll inverter " + String(kz));
              iKeuze=kz;
              actionFlag=47;
              diagNose=true; //
              return;
          } else 
           
          if (strncasecmp(txBuffer+3,"EDIT=",5) == 0) {
            //input can be 10;EDIT=0-AABB; 
            //ws.textAll("received " + String( (char*)data) + "<br>"); 
              int kz = String(txBuffer[8]).toInt();
              if ( kz > inverterCount-1 ) {
              ws.textAll("error, no such inverter");
              return;  
              }
              char invid[5];
              for(int i=10;  i<15; i++) { invid[i-10] = txBuffer[i]; }
              ws.textAll("edit inverter " + String(kz));
              ws.textAll("id = " + String(invid));
              strncpy(Inv_Prop[kz].invID, invid, 4);
              String bestand = "/Inv_Prop" + String(kz) + ".str"; // /Inv_Prop0.str
              writeStruct(bestand, kz); // save in SPIFFS 
              return;
          } else 
           
           if (strncasecmp(txBuffer+3,"HEALTH",6) == 0) {  
              ws.textAll("check zb system");
              actionFlag=44; // perform the healthcheck
              diagNose=true;
              return;             
          } else          

//           if (strncasecmp(txBuffer+3,"SRESET",6) == 0) {  
//              ws.textAll("reset server");
//              server.reset(); // perform the serverreset
//              return;             
//          } else 
   
 // ************  rtest mosquitto *******************************          
           if (strncasecmp(txBuffer+3,"TESTMQTT",8) == 0) {  
              ws.textAll("test mosquitto");
              actionFlag=49; // perform the healthcheck
              diagNose=true;
              return;             
          } else 

           if (strncasecmp(txBuffer+3,"CLEAR",5) == 0) {  
              ws.textAll("clearWindow");
              return;             
          } else

          if (strncasecmp(txBuffer+3,"REBOOT_INVERTER=",16) == 0) {
              int kz = String(txBuffer[19]).toInt();
              ws.textAll("reboot inverter " + String(kz));
              if ( kz > inverterCount-1 ) 
              {
                 ws.textAll("error, non-excisting inverter");
                 return;  
              }
                 inverterReboot(kz);
              return;
          } else


//// ********************** heap sensor **************************************
//           if (strncasecmp(txBuffer+3,"Mqtt_stateIDX=",6) == 0) {  
//              int kz = String(txBuffer[10]).toInt()*10;
//              kz+=String(txBuffer[11]).toInt();
//              //kz+=String(txBuffer[12]).toInt();
//              if (kz > 26 || kz < 16 ) {
//                ws.textAll("invalid number 26 - 16");
//                return;
//              }
//              
//              memCheck=kz*1000; // max 450
//              ws.textAll("memCheck = " + String(memCheck) + "<br>");
//              
//              return;
//          } else 

           if (strncasecmp(txBuffer+3,"FILES",5) == 0) {  
              //we do this in the loop
              actionFlag = 46;
              return;             
          
          } else 
 
 
 // ********************** zigbee test new*****************************          
           if (strncasecmp(txBuffer+3,"ZBT=",4) == 0) {  
              ws.textAll("going to send a teststring, len=" + String(len));
              //we do this in the loop
              actionFlag = 45;
              diagNose=true;
               return;             
          } else 

           if (strncasecmp(txBuffer+3,"ERASE",5) == 0) {  
              ws.textAll("going to delete all inverter files");
              String bestand;
              for(int i=0; i<50; i++) 
              {
                  String bestand = "/Inv_Prop" + String(i) + ".str";
                  if (LittleFS.exists(bestand)) 
                  {
                      LittleFS.remove(bestand);
                      ws.textAll("removed file " + bestand);
                  }

              }
              inverterCount = 0;
              basisConfigsave(); // save inverterCount
              ws.textAll("done");
              return;             
          
          } else            
           
           if (strncasecmp(txBuffer+3,"DELETE=",7) == 0) {  
              //input can be 10;DELETE=filename
              String bestand="";
              for(int i=10;  i<len+1; i++) { bestand += String(txBuffer[i]); }
               ws.textAll("bestand = " + bestand); 
              if (LittleFS.exists(bestand)) 
              {
                  ws.textAll("going to delete file " + bestand); 
                  if(!bestand.indexOf("Inv_Prop") == -1 ) 
                  {
                      LittleFS.remove(bestand);
                      ws.textAll("file " + bestand + " removed!"); 
                  } else {
                      ws.textAll("inverterfile not removed, use 10;erase!"); 
                  }
              
              } else 
              { 
                 ws.textAll("no such file");
              }
              return;                      
          } else

      if (strncasecmp(txBuffer+3, "DIAG",4) == 0) // normal operation
      {
         if(diagNose) {
          diagNose = false;
         } else {
          diagNose= true;
         } 
          ws.textAll("set diagnose to " + String(diagNose) );  
// ****************************************************************
      } else      
      
     if (strncasecmp(txBuffer+3, "INIT_N",6) == 0) // normal operation
      {
         ws.textAll("command = " + String(txBuffer) );  
         actionFlag = 21;
         diagNose=true;
         return;
// ***************************************************************
//      } else 
//
//      if (strncasecmp(txBuffer+3, "INIT_P",6) == 0)  // pairing
//      {
//         ws.textAll("command = " + String(txBuffer) );  
//         actionFlag = 22;

#ifdef TEST
      } else 

      if (strncasecmp(txBuffer+3, "TESTINV",7) == 0)  
      {
         ws.textAll("command = " + String(txBuffer) );  
 //          which = String(txBuffer[10]).toInt();
  //         ws.textAll("chosen = " + String(which) );
 
         actionFlag = 122;
#endif      
      
 
      } else {

       
       ws.textAll("unknown command"); 
      }
  
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    //Serial.println("onEvent triggered");
    switch (type) {
      case WS_EVT_CONNECT:
        //Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        //Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        //Serial.println("WebSocket received data");
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
