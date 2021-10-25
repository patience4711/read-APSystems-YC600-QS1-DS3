// hier komen we als een ongeautoriseerde request is gedaan
void handleNotFound(AsyncWebServerRequest *request) {
  
String serverargument = request->url().c_str();
//DebugPrintln("we zijn in handleNotFound");
//DebugPrint("serverargument = "); //DebugPrintln(serverargument);
// als de client van buiten het netwerk komt mag hij niks

String addy = request->client()->remoteIP().toString();
bool intern = false;
String router=WiFi.gatewayIP().toString();
router = router.substring(0, 9);
//DebugPrint("router substring 0-9 = "); //DebugPrintln(router);

if ( addy.indexOf(router) == -1 ) { // indexOf geeft -1 als niet gevonden
  //DebugPrintln("the client comes form outside the network");
  intern = false; 
  } else {
  //DebugPrintln("the client comes form inside the network"); 
  intern = true; }

if ( intern ) {    //DebugPrintln("the request comes from inside the network");
             
        // POLL=;
        if ( serverargument.indexOf("POLL=") > -1) {
              if(Polling)
              {
                 request->send ( 200, "text/plain", "automatic polling, skipping" ); //zend bevestiging
                 return; 
              }
              DebugPrintln("handleNotFound found POLL=");
              int x = serverargument.indexOf("POLL=");
              //ws.textAll("serverargument = " + serverargument);
              //ws.textAll("sub = " + serverargument.substring(6,7));
              int inv = serverargument.substring(6,7).toInt();
              //ws.textAll("inv= " + String(inv));
              String teZenden = "polling inverternr " + String(inv);
              if(inv > inverterCount-1 ) {
                  String teZenden="ERROR " + serverargument + " inverter not exists !";
                  request->send ( 200, "text/plain", teZenden ); //zend bevestiging
                  return;
              }               
             iKeuze = inv;
             teZenden = "poll inverter " + String(inv);
             request->send ( 200, "text/plain", teZenden ); //zend bevestiging
             actionFlag = 47; // takes care for the polling
             return;
             }
             
             
             // if we are here, no valid api was found    
             request->send ( 200, "text/plain", "ERROR this link is invalid, go back <--" );//send not found message
             }             

    else { // not intern
      //DebugPrint("\t\t\t\t unauthorized, not from inside the network : ");
      request->send ( 200, "text/plain", "ERROR you are not authorised, go back <--" );//send not found message
    }
}
