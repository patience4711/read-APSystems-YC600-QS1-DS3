void pairOnActionflag() {
//start with setup the coordinator
//can we pair when the radio is up for normal operation
   
   String term = "start pairing inverter sn " + String(Inv_Prop[iKeuze].invSerial);
   Update_Log("pairing", term);
    if( !coordinator_init() ) {
      term="pairing failed, zb system down";
      Update_Log("pairing", term);
      ws.textAll(term);
       return;
    }

  ws.textAll("try pairing inv " + String(iKeuze));
  // now that we know that the radio is up, we don't need to test this in the pairing routine

  if( pairing(iKeuze) ) {
    //DebugPrintln("pairing success, saving configfile");
    String term = "success, inverter got id " + String(Inv_Prop[iKeuze].invID);
    Update_Log("pairing", term);
    ws.textAll(term);  
  } else {
    //Serial.println("pairing failed");
    strncpy(Inv_Prop[iKeuze].invID, "0x0000", 6);
    String term = "failed, inverter got id " + String(Inv_Prop[iKeuze].invID);
    Update_Log("pair", term);
    ws.textAll(term);      
  }
    String bestand = "/Inv_Prop" + String(iKeuze) + ".str"; // /Inv_Prop0.str
    writeStruct(bestand, iKeuze); // alles opslaan in SPIFFS   
   
   //after successfull pairing we issue the command for normal ops
   sendNO();
   checkCoordinator(); // updates the log
}

void handlePair(AsyncWebServerRequest *request) {

     strncpy(Inv_Prop[iKeuze].invID, "1x1111\0", 7); // this value makes the pairing page visable
     
     actionFlag = 60; // we do this because no delay is alowed within a async request
     toSend=FPSTR(WAIT_PAIR);
     toSend.replace("{#}", String(iKeuze));
     request->send(200, "text/html", toSend); //send the html code to the client
}

bool pairing(int which) {
// we call this function when coordinator is up for pairing

char pairCmd[254]={0};
char ecu_id_reverse[13]; //= {ECU_REVERSE()}; // 
ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
//char short_ecu_id_reverse
char infix[] = "FFFF10FFFF";
char outfix[] = "10FFFF";
String toLog ="";
//    String pcmd[6] = {
//    "5",
//    "6700",
//2    pcmd_prefix + "0D0200000F1100" + String(inv_sn) + "FFFF10FFFF" + ecu_id_reverse,
// should be     24020FFFFFFFFFFFFFFFFF14FFFF14 0D0200000F1100 408000158215 FFFF10FFFF 80971B01A3D8
// constructed = 24020FFFFFFFFFFFFFFFFF14FFFF14 0D0200000F1100 408000158215 FFFF10FFFF 80971B01A3D8 OK

//3    pcmd_prefix + "0C0201000F0600" + String(inv_sn),
// should be     24020FFFFFFFFFFFFFFFFF14FFFF14 0C0201000F0600 408000158215
// constructed = 24020FFFFFFFFFFFFFFFFF14FFFF14 0C0201000F0600 408000158215 OK

//4    pcmd_prefix + "0F0102000F1100" + String(inv_sn) + ecu_id.substring(2,4) + ecu_id.substring(0,2) + "10FFFF" + ecu_id_reverse,
// should be 24020FFFFFFFFFFFFFFFFF14FFFF14 0F0102000F1100 408000158215 A3D8 10FFFF 80971B01A3D8
// construct 24020FFFFFFFFFFFFFFFFF14FFFF14 0F0102000F1100 408000158215 A3D8 10FFFF 80971B01A3D8 OK
             
//5    pcmd_prefix + "010103000F0600" + ecu_id_reverse
// should be 24020FFFFFFFFFFFFFFFFF14FFFF14 010103000F0600 80971B01A3D8
// construct 24020FFFFFFFFFFFFFFFFF14FFFF14 010103000F0600 80971B01A3D8 OK

char pairBaseCommand[][254] = {
    "5", // not a command but the total commands
    "2700", // was 67
    "24020FFFFFFFFFFFFFFFFF14FFFF140D0200000F1100", // + String(inv_sn) + "FFFF10FFFF" + ecu_id_reverse,
    "24020FFFFFFFFFFFFFFFFF14FFFF140C0201000F0600",  // + String(inv_sn),
    "24020FFFFFFFFFFFFFFFFF14FFFF140F0102000F1100",  // + String(inv_sn) + ecu_id.substring(2,4) + ecu_id.substring(0,2) + "10FFFF" + ecu_id_reverse,
    "24020FFFFFFFFFFFFFFFFF14FFFF14010103000F0600",  // + ecu_id_reverse

};

//now build the commands

// ***************************** command 2 ********************************************
// now build command 2 this is prefix + "0D0200000F1100" + String(invSerial) + "FFFF10FFFF" + ecu_id_reverse,
// add the inverter serial;
strncat(pairBaseCommand[2], Inv_Prop[which].invSerial, sizeof(Inv_Prop[which].invSerial)); 
delayMicroseconds(250);
//now add the "FFF10FFF"
strncat( pairBaseCommand[2], infix, sizeof(infix) );
//now add ecu_id_reverse 
strncat(pairBaseCommand[2], ecu_id_reverse, sizeof(ecu_id_reverse));
// Serial.println("Cmd 2 constructed = " + String(pairBaseCommand[2]));  // ok

// now build command 3 this is prefix + "0C0201000F0600"  + inv serial,
// add the inverter serial;
//Serial.println("Cmd 3 initial = " + String(pairBaseCommand[3]));
strncat(pairBaseCommand[3], Inv_Prop[which].invSerial,  sizeof(Inv_Prop[which].invSerial));
//Serial.println("Cmd 3 constructed = " + String(pairBaseCommand[3]));  // ok

// now build command 4 this is prefix + "0F0102000F1100"  + invSerial + short ecu_id_reverse, + 10FFF + ecu_id_reverse
// add the inverter serial;
//Serial.println("Cmd 4 initial = " + String(pairBaseCommand[4]));
strncat(pairBaseCommand[4], Inv_Prop[which].invSerial,  sizeof(Inv_Prop[which].invSerial));
strncat(pairBaseCommand[4], ECU_ID + 2, 2); // D8A3011B9780 must be A3D8
strncat(pairBaseCommand[4], ECU_ID, 2);
strncat(pairBaseCommand[4], outfix, sizeof(outfix) );
strncat(pairBaseCommand[4], ecu_id_reverse, sizeof(ecu_id_reverse));
//Serial.println("Cmd 4 constructed = " + String(pairBaseCommand[4]));  // ok

//// now build command 5 this is prefix  + "010103000F0600" + ecu_id_reverse,
//Serial.println("Cmd 5 initial = " + String(pairBaseCommand[5]));
strncat(pairBaseCommand[5], ecu_id_reverse, sizeof(ecu_id_reverse));
//Serial.println("Cmd 5 constructed = " + String(pairBaseCommand[5]));  // ok

// now send the  5 commands
// the first command is the healtcheck so we could do checkZigbeeRadio and if this failes break
// the radiocheck is done already so we can skip cmd 1    
    bool success = false;
    for (int y = 2; y < 6; y++) 
    {
      //cmd 0 tm / 9 alles ok

      strncpy(pairCmd, pairBaseCommand[y], sizeof(pairBaseCommand[y] ));
      //add sln 
      strcpy(pairCmd, strncat(sLen(pairCmd), pairCmd, sizeof(sLen(pairCmd)) + sizeof(pairCmd))); 
      delayMicroseconds(250);
      // add CRC at the end 
      strcpy(pairCmd, strncat(pairCmd, checkSum(pairCmd), sizeof(pairCmd) + sizeof(checkSum(pairCmd))));
      delayMicroseconds(250);
      // send and read
      if(diagNose)ws.textAll("pair command = " + String(pairCmd));      
      ws.textAll("sending paircmd " + String(y));
      //toLog="sending cmd " + String(y);
      //if(Log) Update_Log("pair", toLog);
      #ifdef DEBUG
      Serial.println("pair swap zb");
      swap_to_zb(); // set serial to zb  
      #endif 
      sendZigbee(pairCmd);
      delay(1500); // give the inverter the chance to answer
      //waitSerialAvailable();
      //check if anything was received
      readZigbee();
      #ifdef DEBUG
      swap_to_usb(); // set serial to zb  
      Serial.println("pair swap usb");
      #endif
      if(diagNose) ws.textAll("received : " + String(inMessage) );
      // after sending cmd 3 or 4 we can expect an answer to decode
      if(y == 3 || y == 4) {
        if ( decodePairMessage(which) ) 
          {
             success = true;
          } 
      }
   }
   if(success) {return true; } else { return false;}
}

bool decodePairMessage(int which)
{
// we need the fullincomingMessge and the invSns  invSns = inverterSerial
char messageToDecode[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
char _CC2530_answer_string[] = "44810000";
char _noAnswerFromInverter[32] = "FE0164010064FE034480CD14011F";
char * result;                                 
char temp[13];

//received message with inverter id like : 
//for testing
//strncpy(messageToDecode, "FE1C4481000001013A101414003400622305000008FF1A4080001582153A100E08", 66);
//                          
//                         FE1C 4481000001013A101414002700321401000008FFFF4080001582153A100E9D
//  FE0164020067 FE25448100000F0100001414013100204D03000011408000158215A3D610FFFF80971B01A3D63A100DD9FE1C4481000001013A1014140031007D5003000008FFFF4080001582153A100E82
// the 2nd message is shorter 2524020FFFFFFFFFFFFFFFFF14FFFF140F0102000F1100408000157673A3D810FFFF80971B01A3D8D3

    //Serial.println("inMessage = " + String(inMessage));
    strncpy(messageToDecode, inMessage, strlen(inMessage));  // get rid of the preceeding FE0164020067
    delayMicroseconds(250); //give memset a little bit of time to empty all the buffers
        
    //Serial.println("messageToDecode = " + String(messageToDecode));
    
    if (strcmp(messageToDecode, _noAnswerFromInverter) == 0) // default answer if the asked inverter doesn't send us values we compare the whole message
    {
    ws.textAll("received noAnswerFromInverter code, returning..");
    return false;
    }

    if (strlen(messageToDecode) > 222)
    {
      ws.textAll("no pairing code, returning...");
      return false;   
    }
// the message is shorter so continueing    

 if (!strstr(messageToDecode, Inv_Prop[which].invSerial)) {
    ws.textAll("not found serialnr, returning");
    return false;
 }

  if ( strstr(messageToDecode, Inv_Prop[which].invSerial) ) { 
  result = split(messageToDecode, Inv_Prop[which].invSerial);
  }
//  Serial.println("result after 1st splitting = " + String(result));
// now we keep splitting as long as result contains the serial
  while ( strstr(result, Inv_Prop[which].invSerial) ) 
  { 
  result = split(result, Inv_Prop[which].invSerial);
  }
//  Serial.println("result after splitting = " + String(result));

// now we know that it is what we expect so do the next test

strncpy(temp, result, 4);
temp[4]='\0';
//Serial.println("found invID on MessageToDecode" + String(temp));
memset(&Inv_Prop[which].invID, 0, sizeof(Inv_Prop[which].invID)); //zero out the 
delayMicroseconds(250);  
strncpy(Inv_Prop[which].invID, "0x", 2);
strncat(Inv_Prop[which].invID, temp + 2, 2);
strncat(Inv_Prop[which].invID, temp, 2);
Inv_Prop[which].invID[6] = '\0';
if ( String(temp) == "0000" ) {
//String term = "pairing failed, returning false";
//Update_Log("pairing" , term);
//ws.textAll(term);
//Serial.println("temp = " + String(temp));
return false;    
}
//String term = "success, got invID";
//Update_Log("pairing" , term);
//ws.textAll(term);
return true;
} 
