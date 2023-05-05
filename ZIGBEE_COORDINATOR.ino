bool coordinator(bool normal) { // if true we send the extra command for normal operations
    DebugPrintln("start coordinator");  
    coordinator_init();
    if(normal) sendNO();
    // now check if running
    delay(1000); // to give the ZB the time to start
    if ( checkCoordinator() == 0 ) // can be 0 1 or 2
     
    {
        Update_Log("zigbee" , "ZB coordinator started");
        ws.textAll("ZB coordinator started");
        //busyState = 0;
        ledblink(5,100);
        return true;
      
    } else {
        Update_Log("zigbee" , "starting ZB coordinator failed");
        ws.textAll("starting ZB coordinator failed");
        return false;  
    }
}

void coordinator_init() {
/*
* init the coordinator takes the following procedure
* 1st we send a resetcommand 4 times Sent=FE0141000040
* then we send the following commands
*  0 Sent=FE03260503010321
*  Received=FE0166050062
*  1 Sent=FE0141000040
*  Received=FE064180020202020702C2
*  2 Sent=FE0A26050108FFFF80971B01A3D856
*  Received=FE0166050062
*  3 Sent=FE032605870100A6
*  Received=FE0166050062
*  4 Sent=FE 04 26058302 D8A3 DD  should be ecu_id the fst 2 bytes
*  Received=FE0166050062
*  5 Sent=FE062605840400000100A4
*  Received=FE0166050062
*  6 Sent=FE0D240014050F0001010002000015000020
*  Received=FE0164000065
*  7 Sent=FE00260026
*  8 Sent=FE00670067
*  Received=FE0145C0098D
*  received FE00660066 FE0145C0088C FE0145C0098D F0F8FE0E670000FFFF80971B01A3D8000007090011
*  now we can pair if we want to or else an extra command for retrieving data (normal operation)
*  9 for normal operation we send cmd 9
*  Finished. Heap=26712
*  
*/
    ws.textAll("init zb coordinator");
    zigbeeUp = 11; //initial it is initializing 11, 0=down 1=up
    yield();
    //char s_d[150];
    char ecu_id_reverse[13]; //= {ECU_REVERSE()};
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
    //char initCmd[254]={0};

    // commands for setting up coordinater
    char initCmd[][254] = {
      "2605030103", // ok   this is a ZB_WRITE_CONFIGURATION CMD //changed to 01
      "410000",     // ok   ZB_SYS_RESET_REQ
      "0", // + ecu_id_reverse,  this is a ZB_WRITE_CONFIGURATION CMD
      "2605870100",  //ok 
      "0",  // + ecu_id.substring(0,2) + ecu_id.substring(2,4),
      "2605840400000100", //ok
      "240014050F00010100020000150000",  //AF_REGISTER register an application’s endpoint description
      "2600", //ok ZB_START_REQUEST
    };

    char ecu_short[5]={0};
    strncat(ecu_short, ECU_ID + 2, 2); // D8A3011B9780 must be A3D8
    strncat(ecu_short, ECU_ID, 2);
    ecu_short[5]='\0';
    ws.textAll("ecu_short = " + String(ecu_short) );

    // we start with a hard reset of the zb module
    ZBhardReset();
    delay(500);

    // construct some of the commands
    // ***************************** command 2 ********************************************
    // command 2 this is 26050108FFFF we add ecu_id reversed
    //strncat(initCmd[2], ecu_id_reverse, sizeof(ecu_id_reverse)); 
    //delayMicroseconds(250);
    snprintf(initCmd[2], sizeof(initCmd[2]), "26050108FFFF%s", ecu_id_reverse);
    delayMicroseconds(250);

    // ***************************** command 4 ********************************************
    // command 4 this is 26058302 + ecu_id_short 
    //strncat(initBaseCommand[4], ECU_ID, 2);
    //strncat(initBaseCommand[4], ECU_ID + 2, 2);
    //delayMicroseconds(250);
    snprintf(initCmd[4], sizeof(initCmd[4]), "26058302%s", ecu_short);
    delayMicroseconds(250);
    // send the rest of the commands
    for (int y = 0; y < 8; y++) 
    {
      //cmd 0 tm / 9 alles ok

      if(diagNose) ws.textAll("coordinator init cmd " + String(y));
      //sendZB(initCmd); // here it crashed, but why?
      sendZB(initCmd[y]);
      ledblink(1,50);
      delay(1000); // give the inverter the chance to answer

      // read the incoming message
      //s_d[0]='\0'; // if we don't do this, it crashes
      //char * inMess = readZB(s_d);
      // we could skip the reading as we don't do anything with the message
      // so insteat we could flush the buffer. Would save us the buffer s_d
       if(waitSerialAvailable) empty_serial();
    }
    // now all the commands are send 
}

void sendNO() {
    char noCmd[120] ={0} ;   //  we have to send the 10th command
    char ecu_id_reverse[13]; //= {ECU_REVERSE()};
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);

    // ***************************** command 9 ********************************************
    // now send command 9 this is "2401FFFF1414060001000F1E", + ecu_id_reverse + FBFB1100000D6030FBD3000000000000000004010281FEFE"
    snprintf(noCmd, sizeof(noCmd), "2401FFFF1414060001000F1E%sFBFB1100000D6030FBD3000000000000000004010281FEFE", ecu_id_reverse);
    
    ws.textAll("sending N.O. cmd");

    sendZB(noCmd);

    // read the answer , we don't do anything with that
    // just that readZB shows the answer in console.
    //char s_d[150];
    //char * inMessage = readZB(s_d);
    // we could skip the reading as we don't do anything with the message
    // so insteat we could flush the buffer. Would save us the buffer s_d
     if(waitSerialAvailable) empty_serial();

    //zero out noCmd not sure this has any purpose 
    //memset(&noCmd, 0, sizeof(noCmd)); //zero out 
    //delayMicroseconds(250);
}
