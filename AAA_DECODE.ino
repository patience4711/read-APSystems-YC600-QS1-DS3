// ******************************************************************
//                    decode polling answer
// ******************************************************************
int decodePollAnswer(int which)
{
//Serial.println("1 decodePollAnswer which = " + String(which) );
// we need the fullincomingMessge and the inverterSerial
  char messageToDecode[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
  //char _CC2530_answer_string[] = "4481";
  //char _noAnswerFromInverter[32] = "FE01 6401 00 64 FE03 4480 CD14011F";
  //char noAnswerMess[22] = "FE0164010064FE034480"; //
                        // FE0164010064FE034480 CD14011F
//                       FE0164010064FE034480 E9 14013B
//                       FE0164010064FE034480 E9 14013B
//                       FE0164010064FE034480 00 1401D2 // net voordat er waardes komen

  char s_d[254] = {0};
  uint8_t Message_begin_offset = 0;    

  time_t t_old = 0;
  //float t1[YC600_MAX_NUMBER_OF_INVERTERS] = {0};

  float energy_old[4] = {0}; // energy old
  float energy[4]={0};  // energy new
  float power[4] = {0};
  float energy_total = 0;
  float energy_old_total = 0;
  float time_extracted  = 0;
// incoming message is something like this (4 messages at once)
//FE01 64 01 00 64        request status 6401 00=success
//FE03 44 80 00 14 01 D2    AF_DATA_CONFIRM description: sent by the device to the user after it receives a data request.
//FE03 45 C4 3A10 00 A8  ZDO_SRC_RTG_IND Description: This message is an indication to inform host device the receipt of a source route to a given device

// 44 81 AF_INCOMING_MSG Description: This callback message is in response to incoming data to any of the registered endpoints on this device
//FE72 44 81 0000 06013A101414001F006B20D000005E 408000158215 FBFB51B1042D0F42AA00008E8F00000001200360506704A10000000000200001A25FA80900002E8CA01F00030555073F0303030100000100000000000002000000000000000000000000000000000000000000000000FEFE3A100ED3

//F8FE0164010064 FE03 448000 1401D2 FE03 45 C4 3A1000A8 FE72 4481000 006013A101414002C001ECBE200005E 408000158215 FBFB51B1042B0F43B3000090170000000170030C416704A20000000000200001A85DD98300002ED0161F00030556073F0303030100000100000000000002000000000000000000000000000000000000000000000000FEFE3A100EE0
//FE0164010064FE034480001401D2FE0345C43A1000A8FE724481000006013A101414004C00 
//FE0164010064FE034480
   
if(!*inMessage) return 50; // if empty we return with erroCode 50

    strncpy(messageToDecode, inMessage, strlen(inMessage));
    //delayMicroseconds(250); //give memset a little bit of time to empty all the buffers
    
    // we dont need this char anymore so make it empty
    memset( &inMessage, '\0', sizeof(inMessage) ); //zero out the 
    delayMicroseconds(250);
    
    char *tail;
    // diagnose the received message
    //if(diagNose) ws.textAll( "received " + String(messageToDecode));
//FE0164010064
//FE034480001401D2
//FE0345C43A1000A8
//FE724481000006013A10141400730053CF0100005E408000158215FBFB51B103E50F427200003C0B000000C64069C520690494000000000408000079111FD5007808D6A21F00030557073F0303030100000100000000000000000000000000000000000000000000000000000000000000FEFE3A100E6E


    if(strstr(messageToDecode,  "FE01640100") == NULL) // answer to AF_DATA_REQUEST 00=success
    {
     if(diagNose) ws.textAll("AF_DATA_REQUEST failed");
     return 10;    
    } else
    if (strstr(messageToDecode, "FE03448000") == NULL) //  AF_DATA_CONFIRM the 00 byte = success
    {
      if(diagNose) ws.textAll("no AF_DATA_CONFIRM");
      return 11;
    } else
    if (strstr(messageToDecode, "FE0345C4") == NULL) //  ZDO_SRC_RTG_IND
    {
      if(diagNose) ws.textAll("no route receipt");
      //return 12; // this command seems facultative
    } else 
    if (strstr(messageToDecode, "4481") == NULL)
    {
      if(diagNose) ws.textAll("no  AF_INCOMING_MSG");
      return 13;
    }
   
   
    if (strlen(messageToDecode) < 223) // this message is not valid inverter data
    {
       if(diagNose) ws.textAll("ignoring, received " + String(messageToDecode) );
       return 15;
    }    
        
    //shorten the message by removing everything before 4481

    tail = split(messageToDecode, "44810000"); // remove the 0000 as well
    //tail = after removing the 1st part
    //06013A101414001F006B20D000005E408000158215FBFB51B1042D0F42AA00008E8F00000001200360506704A10000000000200001A25FA80900002E8CA01F00030555073F0303030100000100000000000002000000000000000000000000000000000000000000000000FEFE3A100ED3F8FE0164010064FE034480001401D2FE0345C43A1000A8FE724481000006013A101414002C001ECBE200005E408000158215FBFB51B1042B0F43B3000090170000000170030C416704A20000000000200001A85DD98300002ED0161F00030556073F0303030100000100000000000002000000000000000000000000000000000000000000000000FEFE3A100EE0
     
    //if(diagNose) ws.textAll("received tail " + String(tail) );

        //NEW VERSION of s_d generation with search string
        memset(&s_d[0], 0, sizeof(s_d)); //zero out all buffers we could work with "messageToDecode"
        delayMicroseconds(250);            //give memset a little bit of time to empty all the buffers
        strncpy(s_d, tail + 30, strlen(tail));
        delayMicroseconds(250); //give memset a little bit of time to empty all the buffers

//      frequency ac voltage and temperature
        //Inv_Data[which].acv = String((float)((extractValue(56, 4, 1, 0, s_d) * ((float)1 / (float)1.3277)) / 4), 1);
        dtostrf((float)((extractValue(56, 4, 1, 0, s_d) * ((float)1 / (float)1.3277)) / 4), 0, 1, Inv_Data[which].acv);

        dtostrf((float)50000000 / extractValue(24, 6, 1, 0, s_d), 0, 1, Inv_Data[which].freq);
         
        dtostrf(extractValue(20, 4, 0.2752F, -258.7F, s_d), 0, 1, Inv_Data[which].heath); 

                  
         
         // ******************  voltage   *****************************************
         //voltage ch1 offset 24
         dtostrf( (extractValue( 48, 2, (float)16, 0, s_d ) + extractValue(46, 1, 1, 0, s_d)) * (float)82.5 / (float)4096, 0, 1, Inv_Data[which].dcv[1]);
         //voltage ch2 offset 27
         dtostrf( (extractValue( 54, 2, (float)16, 0, s_d ) + extractValue(52, 1, 1, 0, s_d)) * (float)82.5 / (float)4096, 0, 1, Inv_Data[which].dcv[0]);
         // ******************  current   *****************************************
         //current ch1 offset 22
         float currentP1 = (extractValue(47, 1, (float)256, 0, s_d) + extractValue(44, 2, 1, 0, s_d)) * (float)27.5 / (float)4096; //[A]
         dtostrf(currentP1, 0, 1, Inv_Data[which].dcc[1]);
         //current ch1 offset 25
         float currentP0 = (extractValue(53, 1, (float)256, 0, s_d) + extractValue(50, 2, 1, 0, s_d)) * (float)27.5 / (float)4096; //[A]
         dtostrf(currentP0, 0, 1, Inv_Data[which].dcc[0]);

        //********************************************************************************************
        //                                     SQ1
        //********************************************************************************************
        if(Inv_Prop[which].invType == 1) //SQ1 inverter
        {
          //these are the values of channel 2 and 3 of the sq1 according to electrosun911
          //offset 21 -> byte for voltage ch3
          dtostrf( (extractValue( 42, 2, (float)16, 0, s_d ) + extractValue(40, 1, 1, 0, s_d)) * (float)82.5 / (float)4096, 0, 1, Inv_Data[which].dcv[2]);          
          //offset 18 -> byte for voltage ch4
          dtostrf( (extractValue( 36, 2, (float)16, 0, s_d ) + extractValue(34, 1, 1, 0, s_d)) * (float)82.5 / (float)4096, 0, 1, Inv_Data[which].dcv[3]);

          // ***************************  current  *****************************************
          //offset 19 and 20 for current and status ch3
          float currentP2 = ( extractValue( 41, 1, (float)256, 0, s_d ) + extractValue(38, 2, 1, 0, s_d)) * (float)27.5 / (float)4096; //[A]
          dtostrf(currentP2, 0, 1, Inv_Data[which].dcc[2]);
    
          //offset 16 and 17 for current and status ch4
          float currentP3 = ( extractValue( 35, 1, (float)256, 0, s_d ) + extractValue(32, 2, 1, 0, s_d)) * (float)27.5 / (float)4096; //[A]
          dtostrf(currentP3, 0, 1, Inv_Data[which].dcc[3]);
        }  
            
            yield();

// **********************************************************************
//               calculation of the power per panel
// **********************************************************************
// 1st the time period           

//extractValue: float extractValue(startPosition, lengthOfValue, slopeOfValue, offsetOfValue, char toDecode[CC2530_MAX_SERIAL_BUFFER_SIZE])

    //t_new is globally defined so we remember the last. With the new we can calculate the timeperiod
    t_old = t_new[which]; // save the old value in t_old not need to be global but t_new must global
    
    t_new[which] = now(); // we get the timestamp from the system and not from the inverters
    
    if(diagNose) ws.textAll("the timespan = " + String(t_new[which] - t_old) );

    // now for each channel 
    
    int offst = 74; // this is incremented with 10
    float total = 0;
    // for every panel of inverter which we go through this loop
   
    #ifndef DEBUG
    //Serial.print("power/panel: ");    
    #endif
    ws.textAll("* * * * polled inverter " + String(which) + " * * * *");
    for(int x = 0; x < 4; x++ ) 
    {   
         if(Inv_Prop[which].conPanels[x]) { // is this panel connected ? otherwise skip
            
            // first store the last value of energy_new in energy_old
            energy_old[x] = energy_new[which][x]; // en_new (per inverter per panel needs to be global
            
            // now we extract energy_new[which][x] and remember this value
            energy_new[which][x] = extractValue(offst+x*10, 6, 1, 0, s_d); // offset 74 todays module energy channel 0
            
            //calculate the daily energy for this panel
            //energy[x] = (energy_new[which][x] * 8.311F / (float)3600 / (float)1000); //[kWh]
            energy[x] = (energy_new[which][x] * 8.311F / (float)3600); //[Wh]
            //calculate the power
            power[x] = ((energy_new[which][x] - energy_old[x]) * 8.311F / (t_new[which] - t_old)); //[W]
            if (power[x] < 0) power[x] = 0; // otherwise negative values may be reported
            //Inv_Data[which].power[x] = String(power[x], 1);
            dtostrf( power[x], 0, 1, Inv_Data[which].power[x]); //
            
            total += power[x]; // totalize the power for all panels
            
            energy_total += energy[x]; //totalize the daily energy
            energy_old_total += energy_old[x]; // when this is 0 this is a first poll
            yield();
            if(diagNose) 
            {
                //ws.textAll("powercalculations for panel " + String(x) );
                //Serial.println("energy_old = " + String(energy_old[x]) );
                //Serial.println("energy_new p" + String(x) + " " + String(energy_new[which][x]) );
                ws.textAll("panel " + String(x) + "  power " + String(power[x]) + "  energy today " + String(energy[x]) );
                //ws.textAll("power " + String(power[x]) );            
                //Serial.println("Inv_Data.power p" + String(x) + " = " + String(Inv_Data[which].power[x]) );
            }     

        } else {
          if(diagNose) ws.textAll("no panel " + String(x));
        }
    }           
     ws.textAll("total inverter: pwr " + String(total) + "  en: " + String(energy_total) + "\n");
     dtostrf( total, 0, 1, Inv_Data[which].power[4]); //
    // to prevent 0 values for total energy we update this value only when it increased. (reset at wake from nightmode
    if(Inv_Data[which].totalEnergy < energy_total) Inv_Data[which].totalEnergy = energy_total; //only when the new value is higher
    
    if(energy_old_total == 0) 
    {
    //when this is 0 all values are 0 and should not be reported via mqtt
    ws.textAll("first poll, return false");
    return 2;  
    }
    return 0;
}       

//// ******************************************************************
////                    decode polling answer
//// ******************************************************************
//int decodePollAnswer(int which)
//{
////Serial.println("1 decodePollAnswer which = " + String(which) );
//// we need the fullincomingMessge and the inverterSerial
//  char messageToDecode[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
//  //char _CC2530_answer_string[] = "4481";
//  //char _noAnswerFromInverter[32] = "FE01 6401 00 64 FE03 4480 CD14011F";
//  //char noAnswerMess[22] = "FE0164010064FE034480"; //
//                        // FE0164010064FE034480 CD14011F
////                       FE0164010064FE034480 E9 14013B
////                       FE0164010064FE034480 E9 14013B
////                       FE0164010064FE034480 00 1401D2 // net voordat er waardes komen
//
//  char s_d[254] = {0};
//  uint8_t Message_begin_offset = 0;    
//
//  time_t t_old = 0;
//  //float t1[YC600_MAX_NUMBER_OF_INVERTERS] = {0};
//
//  float energy_old[4] = {0}; // energy old
//  float energy[4]={0};  // energy new
//  float power[4] = {0};
//  float energy_total = 0;
//  float energy_old_total = 0;
//  float time_extracted  = 0;
//// incoming message is something like this (4 messages at once)
////FE01 64 01 00 64        request status 6401 00=success
////FE03 44 80 00 14 01 D2    AF_DATA_CONFIRM description: sent by the device to the user after it receives a data request.
////FE03 45 C4 3A10 00 A8  ZDO_SRC_RTG_IND Description: This message is an indication to inform host device the receipt of a source route to a given device
//
//// 44 81 AF_INCOMING_MSG Description: This callback message is in response to incoming data to any of the registered endpoints on this device
////FE72 44 81 0000 06013A101414001F006B20D000005E 408000158215 FBFB51B1042D0F42AA00008E8F00000001200360506704A10000000000200001A25FA80900002E8CA01F00030555073F0303030100000100000000000002000000000000000000000000000000000000000000000000FEFE3A100ED3
//
////F8FE0164010064 FE03 448000 1401D2 FE03 45 C4 3A1000A8 FE72 4481000 006013A101414002C001ECBE200005E 408000158215 FBFB51B1042B0F43B3000090170000000170030C416704A20000000000200001A85DD98300002ED0161F00030556073F0303030100000100000000000002000000000000000000000000000000000000000000000000FEFE3A100EE0
////FE0164010064FE034480001401D2FE0345C43A1000A8FE724481000006013A101414004C00 
////FE0164010064FE034480
//   
//if(!*inMessage) return 50; // if empty we return with erroCode 50
//
//    char *tail;
//// now we can check if the testString is present
//    if (strstr(inMessage, "448100") != NULL) 
//    {
//       ws.textAll("found 448100, decoding...");
//       ws.textAll("inMessage after strstr: " + String(inMessage));
//       //short the message by removing everything before 44810000
//       tail = split(inMessage, "44810000"); // remove the 0000 as well
//       if(diagNose) ws.textAll("tail = " + String(tail) );
//        // we don't need this char anymore so make it empty
//        memset( &inMessage, '\0', sizeof(inMessage) ); //zero out the 
//        delayMicroseconds(250);
//    } 
//    else
//    {
//    // we have to find out whats in the string  
//    strncpy(messageToDecode, inMessage, 20);
//    // we dont need this char anymore so make it empty
//    memset( &inMessage, '\0', sizeof(inMessage) ); //zero out the 
//    delayMicroseconds(250);
//    
//
//    // diagnose the received message
//    //if(diagNose) ws.textAll( "received " + String(messageToDecode));
////FE0164010064
////FE034480001401D2
////FE0345C43A1000A8
////FE724481000006013A10141400730053CF0100005E408000158215FBFB51B103E50F427200003C0B000000C64069C520690494000000000408000079111FD5007808D6A21F00030557073F0303030100000100000000000000000000000000000000000000000000000000000000000000FEFE3A100E6E
//
//
//    if(strstr(messageToDecode,  "FE01640100") == NULL) // answer to AF_DATA_REQUEST 00=success
//    {
//     if(diagNose) ws.textAll("AF_DATA_REQUEST failed");
//     return 10;    
//    } else
//    if (strstr(messageToDecode, "FE03448000") == NULL) //  AF_DATA_CONFIRM the 00 byte = success
//    {
//      if(diagNose) ws.textAll("no AF_DATA_CONFIRM");
//      return 11;
//    } else
//    if (strstr(messageToDecode, "FE0345C4") == NULL) //  ZDO_SRC_RTG_IND
//    {
//      if(diagNose) ws.textAll("no route receipt");
//      //return 12; // this command seems facultative
//    } else 
//    //if (strstr(messageToDecode, "4481") == NULL) //we know that
//    {
//      if(diagNose) ws.textAll("no  AF_INCOMING_MSG");
//      return 13;
//    }
//  } 
//// if we are here we know that it's a valid poll string, maybe this test is not needed   
////    if (strlen(messageToDecode) < 223) // this message is not valid inverter data
////    {
////       if(diagNose) ws.textAll("ignoring, received " + String(messageToDecode) );
////       return 15;
////    }    
//         
//    //shorten the message by removing everything before 4481
//
//    //tail = split(messageToDecode, "44810000"); // remove the 0000 as well
//    //tail = after removing the 1st part
//    //06013A101414001F006B20D000005E408000158215FBFB51B1042D0F42AA00008E8F00000001200360506704A10000000000200001A25FA80900002E8CA01F00030555073F0303030100000100000000000002000000000000000000000000000000000000000000000000FEFE3A100ED3F8FE0164010064FE034480001401D2FE0345C43A1000A8FE724481000006013A101414002C001ECBE200005E408000158215FBFB51B1042B0F43B3000090170000000170030C416704A20000000000200001A85DD98300002ED0161F00030556073F0303030100000100000000000002000000000000000000000000000000000000000000000000FEFE3A100EE0
//     
//    //NEW VERSION of s_d generation with search string
//        memset(&s_d[0], 0, sizeof(s_d)); //zero out all buffers we could work with "messageToDecode"
//        delayMicroseconds(250);            //give memset a little bit of time to empty all the buffers
//        strncpy(s_d, tail + 30, strlen(tail));
//        delayMicroseconds(250); //give memset a little bit of time to empty all the buffers
//
////      frequency ac voltage and temperature
//        //Inv_Data[which].acv = String((float)((extractValue(56, 4, 1, 0, s_d) * ((float)1 / (float)1.3277)) / 4), 1);
//        dtostrf((float)((extractValue(56, 4, 1, 0, s_d) * ((float)1 / (float)1.3277)) / 4), 0, 1, Inv_Data[which].acv);
//
//        dtostrf((float)50000000 / extractValue(24, 6, 1, 0, s_d), 0, 1, Inv_Data[which].freq);
//         
//        dtostrf(extractValue(20, 4, 0.2752F, -258.7F, s_d), 0, 1, Inv_Data[which].heath); 
//
//                  
//         
//         // ******************  voltage   *****************************************
//         //voltage ch1 offset 24
//         dtostrf( (extractValue( 48, 2, (float)16, 0, s_d ) + extractValue(46, 1, 1, 0, s_d)) * (float)82.5 / (float)4096, 0, 1, Inv_Data[which].dcv[1]);
//         //voltage ch2 offset 27
//         dtostrf( (extractValue( 54, 2, (float)16, 0, s_d ) + extractValue(52, 1, 1, 0, s_d)) * (float)82.5 / (float)4096, 0, 1, Inv_Data[which].dcv[0]);
//         // ******************  current   *****************************************
//         //current ch1 offset 22
//         float currentP1 = (extractValue(47, 1, (float)256, 0, s_d) + extractValue(44, 2, 1, 0, s_d)) * (float)27.5 / (float)4096; //[A]
//         dtostrf(currentP1, 0, 1, Inv_Data[which].dcc[1]);
//         //current ch1 offset 25
//         float currentP0 = (extractValue(53, 1, (float)256, 0, s_d) + extractValue(50, 2, 1, 0, s_d)) * (float)27.5 / (float)4096; //[A]
//         dtostrf(currentP0, 0, 1, Inv_Data[which].dcc[0]);
//
//        //********************************************************************************************
//        //                                     SQ1
//        //********************************************************************************************
//        if(Inv_Prop[which].invType == 1) //SQ1 inverter
//        {
//          //these are the values of channel 2 and 3 of the sq1 according to electrosun911
//          //offset 21 -> byte for voltage ch3
//          dtostrf( (extractValue( 42, 2, (float)16, 0, s_d ) + extractValue(40, 1, 1, 0, s_d)) * (float)82.5 / (float)4096, 0, 1, Inv_Data[which].dcv[2]);          
//          //offset 18 -> byte for voltage ch4
//          dtostrf( (extractValue( 36, 2, (float)16, 0, s_d ) + extractValue(34, 1, 1, 0, s_d)) * (float)82.5 / (float)4096, 0, 1, Inv_Data[which].dcv[3]);
//
//          // ***************************  current  *****************************************
//          //offset 19 and 20 for current and status ch3
//          float currentP2 = ( extractValue( 41, 1, (float)256, 0, s_d ) + extractValue(38, 2, 1, 0, s_d)) * (float)27.5 / (float)4096; //[A]
//          dtostrf(currentP2, 0, 1, Inv_Data[which].dcc[2]);
//    
//          //offset 16 and 17 for current and status ch4
//          float currentP3 = ( extractValue( 35, 1, (float)256, 0, s_d ) + extractValue(32, 2, 1, 0, s_d)) * (float)27.5 / (float)4096; //[A]
//          dtostrf(currentP3, 0, 1, Inv_Data[which].dcc[3]);
//        }  
//            
//            yield();
//
//// **********************************************************************
////               calculation of the power per panel
//// **********************************************************************
//// 1st the time period           
//
////extractValue: float extractValue(startPosition, lengthOfValue, slopeOfValue, offsetOfValue, char toDecode[CC2530_MAX_SERIAL_BUFFER_SIZE])
//
//    //t_new is globally defined so we remember the last. With the new we can calculate the timeperiod
//    t_old = t_new[which]; // save the old value in t_old not need to be global but t_new must global
//    
//    t_new[which] = now(); // we get the timestamp from the system and not from the inverters
//    
//    if(diagNose) ws.textAll("the timespan = " + String(t_new[which] - t_old) );
//
//    // now for each channel 
//    
//    int offst = 74; // this is incremented with 10
//    float total = 0;
//    // for every panel of inverter which we go through this loop
//   
//    #ifndef DEBUG
//    //Serial.print("power/panel: ");    
//    #endif
//    if(diagNose) ws.textAll("* * * * data inverter #" + String(which) + " * * * *");
//    for(int x = 0; x < 4; x++ ) 
//    {   
//         if(Inv_Prop[which].conPanels[x]) { // is this panel connected ? otherwise skip
//            
//            // first store the last value of energy_new in energy_old
//            energy_old[x] = energy_new[which][x]; // en_new (per inverter per panel needs to be global
//            
//            // now we extract energy_new[which][x] and remember this value
//            energy_new[which][x] = extractValue(offst+x*10, 6, 1, 0, s_d); // offset 74 todays module energy channel 0
//            
//            //calculate the daily energy for this panel
//            //energy[x] = (energy_new[which][x] * 8.311F / (float)3600 / (float)1000); //[kWh]
//            energy[x] = (energy_new[which][x] * 8.311F / (float)3600); //[Wh]
//            //calculate the power
//            power[x] = ((energy_new[which][x] - energy_old[x]) * 8.311F / (t_new[which] - t_old)); //[W]
//            if (power[x] < 0) power[x] = 0; // otherwise negative values may be reported
//            //Inv_Data[which].power[x] = String(power[x], 1);
//            dtostrf( power[x], 0, 1, Inv_Data[which].power[x]); //
//            
//            total += power[x]; // totalize the power for all panels
//            
//            energy_total += energy[x]; //totalize the daily energy
//            energy_old_total += energy_old[x]; // when this is 0 this is a first poll
//            yield();
//            if(diagNose) 
//            {
//                //ws.textAll("powercalculations for panel " + String(x) );
//                //Serial.println("energy_old = " + String(energy_old[x]) );
//                //Serial.println("energy_new p" + String(x) + " " + String(energy_new[which][x]) );
//                ws.textAll("panel " + String(x) + "  power " + String(power[x]) + "  energy today " + String(energy[x]) );
//                //ws.textAll("power " + String(power[x]) );            
//                //Serial.println("Inv_Data.power p" + String(x) + " = " + String(Inv_Data[which].power[x]) );
//            }     
//
//        } else {
//          if(diagNose) ws.textAll("no panel " + String(x));
//        }
//    }           
//     ws.textAll("total inverter: pwr " + String(total) + "  en: " + String(energy_total) + "\n");
//     dtostrf( total, 0, 1, Inv_Data[which].power[4]); //
//    // to prevent 0 values for total energy we update this value only when it increased. (reset at wake from nightmode
//    if(Inv_Data[which].totalEnergy < energy_total) Inv_Data[which].totalEnergy = energy_total; //only when the new value is higher
//    
//    if(energy_old_total == 0) 
//    {
//    //when this is 0 all values are 0 and should not be reported via mqtt
//    ws.textAll("first poll, return false");
//    return 2;  
//    }
//    return 0;
//}       

char *split(char *str, const char *delim)
{
    char *p = strstr(str, delim);

    if (p == NULL)
        return NULL; // delimiter not found

    *p = '\0';                // terminate string after head
    return p + strlen(delim); // return tail substring
}

// *******************************************************************************************************************
//                             extract values
// *******************************************************************************************************************
float extractValue(uint8_t startPosition, uint8_t valueLength, float valueSlope, float valueOffset, char toDecode[CC2530_MAX_SERIAL_BUFFER_SIZE])
{
//    Serial.println("re declare tempMsgBuffer");
char tempMsgBuffer[64] = {0}; // was 254
yield();
    
    memset(&tempMsgBuffer[0], 0, sizeof(tempMsgBuffer)); //zero out all buffers we could work with "messageToDecode"
    delayMicroseconds(250);                              //give memset a little bit of time to empty all the buffers
    strncpy(tempMsgBuffer, toDecode + startPosition, valueLength);
    delayMicroseconds(250); //give memset a little bit of time to empty all the buffers

    yield();
    return (valueSlope * (float)StrToHex(tempMsgBuffer)) + valueOffset;
}

// ************************************************************************************
//                mqtt send polled data
// ************************************************************************************
void mqttPoll(int which) {
// we polled the inverter which end mqtt the data

String toMQTT = "{\"inv_serial\":\"" + String(Inv_Prop[which].invSerial) + "\"";

// the json to domoticz must be something like {"idx" : 7, "nvalue" : 0,"svalue" : "90;2975.00"}
// does it work with the serial on the json?
//sValue is something like "svalue":"20;30; / so we add a "}   
//  "command": "udevice", // default so no meed to use
// "idx" : 7,
// "nvalue" : 0,
// "svalue" : "90;2975.00"  
   
   if( Mqtt_outTopic == "domoticz/in" ) 
    { 
       
       String sValue="\"svalue\":\""; // "svalue":"
       sValue += String(Inv_Data[which].power[4]) + ";" + String(Inv_Data[which].totalEnergy, 2) ; //total of the 2 or 4
       toMQTT += ",\"idx\":" + String(Inv_Prop[which].invIdx) + ",\"nvalue\":0," + sValue + "\"}";
       //Serial.println("we publish to domoticz, mess is : " + toMQTT);
    }  else { 
        // for not domoticz we have a different mqtt string how does this look?
       toMQTT += ",\"temp\":\"" + String(Inv_Data[which].heath) + "\",\"p0\":\"" + String(Inv_Data[which].power[0]) + "\",\"p1\":\"" + String(Inv_Data[which].power[1]) + "\"";
       if( Inv_Prop[which].invType == 1 ) {
       toMQTT += ",\"p2\":\"" + String(Inv_Data[which].power[2]) + "\",\"p3\":\"" + String(Inv_Data[which].power[3])  +  "\"";
       }
       toMQTT += ",\"energy\":\"" + String(Inv_Data[which].totalEnergy, 1) + "\"";
       //Serial.println("we do not publish to domoticz, mess is : " + toMQTT);    
       toMQTT += "}";  
       }
   //DebugPrintln("mqtt mess :"); //DebugPrintln(toMQTT);
   MQTT_Client.publish ( Mqtt_outTopic.c_str(), toMQTT.c_str() );
 }

 
