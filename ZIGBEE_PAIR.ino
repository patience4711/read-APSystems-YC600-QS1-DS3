void pairOnActionflag() {
     // we do this in the loop (outside a server request)
     //this because no delay is alowed within a async request
     //start with setup the coordinator in pairing mode
     String term = "start pairing inverter sn " + String(Inv_Prop[iKeuze].invSerial);
     #ifdef LOG 
     Update_Log(4, "start"); 
     #endif
     if( !coordinator(false) ) {
        #ifdef LOG 
        Update_Log(4, "ZB down"); 
        #endif
        ws.textAll(F("pairing failed, zb system down"));
        return;
     }
  
     ws.textAll("trying pair inv " + String(iKeuze));
    // now we know that the radio is up for pairing
  
    if( pairing(iKeuze) ) {
       String term = "success, inverter got id " + String(Inv_Prop[iKeuze].invID);
       #ifdef LOG 
       Update_Log(4, "success"); 
       #endif
       ws.textAll(term);  
    } else {
      // pairing failed
       strncpy(Inv_Prop[iKeuze].invID, "0000", 4);
       String term = "failed, inverter got id " + String(Inv_Prop[iKeuze].invID);
       #ifdef LOG 
       Update_Log(4, "failed"); 
       #endif
       ws.textAll(term);      
    }
       String bestand = "/Inv_Prop" + String(iKeuze) + ".str"; // /Inv_Prop0.str
       writeStruct(bestand, iKeuze); // alles opslaan in SPIFFS   
     
       //after successfull pairing we issue the command for normal ops
       sendNO();
       checkCoordinator(); // updates the log
}

void handlePair(AsyncWebServerRequest *request) {

     strncpy(Inv_Prop[iKeuze].invID, "1111", 4); // this value makes the pairing page visable
     
     actionFlag = 60; // we do this because no delay is alowed within a async request
     toSend=FPSTR(WAIT_PAIR);
     toSend.replace("{#}", String(iKeuze));
     request->send(200, "text/html", toSend); //send the html code to the client
}

bool pairing(int which) {
    // we call this function when coordinator is up for pairing
    char ecu_id_reverse[13]; 
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
    //char short_ecu_id_reverse
    char ecu_short[5]={0};
    strncat(ecu_short, ECU_ID + 2, 2); // D8A3011B9780 must be A3D8
    strncat(ecu_short, ECU_ID, 2);
    //ecu_short[5]='\0'; strncat is null terminated
    ws.textAll("ecu_short = " + String(ecu_short) );
    delay(1000);   

    char pairCmd[4][254];
    //now build the commands
    // build command 0 this is "24020FFFFFFFFFFFFFFFFF14FFFF14" + "0D0200000F1100" + String(invSerial) + "FFFF10FFFF" + ecu_id_reverse
    snprintf(pairCmd[0], sizeof(pairCmd[0]), "24020FFFFFFFFFFFFFFFFF14FFFF140D0200000F1100%sFFFF10FFFF%s", Inv_Prop[which].invSerial , ecu_id_reverse);
    delayMicroseconds(250);
    // build command 1 this is "24020FFFFFFFFFFFFFFFFF14FFFF14" + "0C0201000F0600"  + inv serial,
    snprintf(pairCmd[1], sizeof(pairCmd[1]), "24020FFFFFFFFFFFFFFFFF14FFFF140C0201000F0600%s", Inv_Prop[which].invSerial );
    delayMicroseconds(250);
    // build command 2 this is "24020FFFFFFFFFFFFFFFFF14FFFF14" + "0F0102000F1100"  + invSerial + short ecu_id_reverse, + 10FFF + ecu_id_reverse
    snprintf(pairCmd[2], sizeof(pairCmd[2]), "24020FFFFFFFFFFFFFFFFF14FFFF140F0102000F1100%s%s10FFFF%s", Inv_Prop[which].invSerial, ecu_short, ecu_id_reverse);
    delayMicroseconds(250);
    // now build command 3 this is "24020FFFFFFFFFFFFFFFFF14FFFF14"  + "010103000F0600" + ecu_id_reverse,
    snprintf(pairCmd[3], sizeof(pairCmd[3]), "24020FFFFFFFFFFFFFFFFF14FFFF14010103000F0600%s", ecu_id_reverse);
    delayMicroseconds(250);

    // the coordinator for pairing is up    
    bool success = false;
    // now send the 4 commands and decode answer 1 and 2
    for (int y = 0; y < 4; y++) 
    {
      // send 
      if(diagNose)ws.textAll("pair command = " + String(pairCmd[y]));      
      ws.textAll("sending paircmd " + String(y));

      sendZB(pairCmd[y]);
      delay(1500); // give the inverter the chance to answer

      // after sending cmd 1 or 2 we can expect an answer to decode
      // we let decodePairMessage retrieve the answer then
      // the other answers are flushed
      if(y == 1 || y == 2) {
          // if y 1 or 2 we catch and decode the answer
          if ( decodePairMessage(which) ) 
            {
               success = true;
            } 
      } else { 
      // if not y 1 or 2 we flush the received message
      if(waitSerialAvailable) empty_serial();  
      }
   }
      //now all 4 commands have been sent
      if(success) { return true; } else { return false;}  // when paired 3A10
}


bool decodePairMessage(int which)
    {
    // this function retrieves the pairinganswers 1 and 2 and tries to find the serialnr
    // if the serial is found, we extract the inverters short id and save it

    char _CC2530_answer_string[] = "44810000";
    //char _noAnswerFromInverter[32] = "FE0164010064FE034480CD14011F";
    char * result;                                 
    char temp[13];
    char s_d[250]={0};

    if(diagNose) ws.textAll("analyzing paircmd ");  

    char messageToDecode[254]={0};  
    strcpy(messageToDecode, readZB(s_d));
    
    ws.textAll("decoding : " + String(messageToDecode));
//    if (readCounter == 0 || strlen(messageToDecode) < 6 ){
//        if(diagNose) ws.textAll("no pairing answer ");
//        messageToDecode[0]='\0'; 
//        return false; //no answer
//    }
    // here i made some changes, before each return make messageToDecode null and 
//    //if (strcmp(messageToDecode, _noAnswerFromInverter) == 0) // default answer if the asked inverter doesn't send us values we compare the whole message
//    if (strcmp(messageToDecode, "FE0164010064FE034480CD14011F") == 0)
//    {                            
//    ws.textAll("received noAnswerFromInverter code, returning..");
//    messageToDecode[0]='\0';
//    return false;
//    }
    // can we conclude that when the message is shorter than rc=42 it is not valid?
    if (strlen(messageToDecode) > 222 || readCounter < 70 || strlen(messageToDecode) < 6)
    {
      ws.textAll("no valid pairing code, returning...");
      messageToDecode[0]='\0';
      return false;   
    }
    // the message is shorter so continue    

    if (!strstr(messageToDecode, Inv_Prop[which].invSerial)) {
      ws.textAll("not found serialnr, returning");
      messageToDecode[0]='\0'; // this should be kept
      return false;
    }
    //  an answer could be like FE0164020067 FE25448100000F0100001414013100204D03000011408000158215A3D610FFFF80971B01A3D63A100DD9FE1C4481000001013A1014140031007D5003000008FFFF4080001582153A100E82
    // it contains the serialnr a few times, we are looking for the 2 bytes right after the last serialnr
    if ( strstr(messageToDecode, Inv_Prop[which].invSerial) ) { 
        result = split(messageToDecode, Inv_Prop[which].invSerial);
    }

    // now we keep splitting as long as result contains the serial
    while ( strstr(result, Inv_Prop[which].invSerial) ) 
    { 
    result = split(result, Inv_Prop[which].invSerial);
    }

    // now we know that it is what we expect: A string after the last occurence of serialnr
    // the shortid is right after the last occurence of serialnr but this can also be the
    // ECU-shortid id when the inverter doesn't exists
    // if(diagNose) ws.textAll("found invID in MessageToDecode");
    memset(&Inv_Prop[which].invID, 0, sizeof(Inv_Prop[which].invID)); //zero out 
    delayMicroseconds(250);  
//    // check if it is not the ECU-shortid
//    char ecu_short[5]={0};
//    strncat(ecu_short, ECU_ID + 2, 2); // D8A3011B9780 must be A3D8
//    strncat(ecu_short, ECU_ID, 2);
//    
//    if(strcmp(result, ecu_short) == 0) {
//      if(diagNose) ws.textAll("invID equals ECU-Short, returning false");
//      return false;
//    }
    // so result is not equal to ECU-shortid
    strncpy(Inv_Prop[which].invID, result, 4);

    if(diagNose) ws.textAll("found invID = " + String(Inv_Prop[which].invID));
    if ( String(Inv_Prop[which].invID) == "0000" ) 
    {
       return false;    
    }
       return true;
} 
