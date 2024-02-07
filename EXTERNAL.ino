// find out if the request comes from inside the network
//check if the first 9 characters of the router's ip ( 192.168.0 ) is in the url
bool checkRemote(String url) {
    if(securityLevel==0) return false; //no security so never remote
    if ( url.indexOf(WiFi.gatewayIP().toString().substring(0, securityLevel)) == -1 ) return true; else return false;
}

// we come here when an unknown request is done

void handleNotFound(AsyncWebServerRequest *request) {
  
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
              //("handleNotFound found POLL=");
              int x = serverUrl.indexOf("POLL=");
              int inv = serverUrl.substring(6,7).toInt();
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
