// find out if the request comes from inside the network
bool checkRemote(String url) {
    //Serial.println("checkRemote the url = " + url);  
    //String router=WiFi.gatewayIP().toString();
    //Serial.println("routers address = " + router); 
    //router = router.substring(0, 9);
    //if ( url.indexOf(router) == -1 ) return true; else return false; 
    if ( url.indexOf(WiFi.gatewayIP().toString().substring(0, 9)) == -1 ) return true; else return false;
}

// we come here when an unknown request is done

void handleNotFound(AsyncWebServerRequest *request) {
  
//String serverargument = request->url().c_str();
////DebugPrintln("we are in handleNotFound");
////DebugPrint("serverargument = "); //DebugPrintln(serverargument);
//// if the client is outside the network he is can do nothing
//
//String addy = request->client()->remoteIP().toString();
//bool intern = false;
//String router=WiFi.gatewayIP().toString();
//router = router.substring(0, 9);
////DebugPrint("router substring 0-9 = "); //DebugPrintln(router);
//
//if ( addy.indexOf(router) == -1 ) { // indexOf geeft -1 als niet gevonden
//  //DebugPrintln("the client comes form outside the network");
//  intern = false; 
//  } else {
//  //DebugPrintln("the client comes form inside the network"); 
//  intern = true; }

bool intern = false;
if(!checkRemote( request->client()->remoteIP().toString()) ) intern = true;

// **************************************************************************
//             R E S T R I C T E D   A R E A
// **************************************************************************
// access only from inside the network

if ( intern ) {    //DebugPrintln("the request comes from inside the network");
        String serverUrl = request->url().c_str();             
        // POLL=;
        if ( serverUrl.indexOf("POLL=") > -1) {
              if(Polling)
              {
                 request->send ( 200, "text/plain", "automatic polling, skipping" ); //zend bevestiging
                 return; 
              }
              DebugPrintln("handleNotFound found POLL=");
              int x = serverUrl.indexOf("POLL=");
              //ws.textAll("serverUrl = " + serverUrl);
              //ws.textAll("sub = " + serverUrl.substring(6,7));
              int inv = serverUrl.substring(6,7).toInt();
              //ws.textAll("inv= " + String(inv));
              String teZenden = "polling inverternr " + String(inv);
              if(inv > inverterCount-1 ) {
                  String teZenden="ERROR " + serverUrl + " inverter not exists !";
                  request->send ( 200, "text/plain", teZenden ); 
                  return;
              }               
             iKeuze = inv;
             teZenden = "polled inverter " + String(inv);
             request->send ( 200, "text/plain", teZenden ); 
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
