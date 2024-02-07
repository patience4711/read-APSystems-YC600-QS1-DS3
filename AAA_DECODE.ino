// ******************************************************************
//                    decode polling answer
// ******************************************************************
int decodePollAnswer(int which)
{
//Serial.println("1 decodePollAnswer which = " + String(which) );
// we need the fullincomingMessge and the inverterSerial
  char messageToDecode[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};

  char s_d[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
  uint8_t Message_begin_offset = 0;    

  int t_old = 0;
 
  float en_old[4] = {0}; // energy old
  float energy_old_total = 0;
  int t_extr  = 0;
  int ts = 0;
  bool resetFlag = false;
  float total_pwr = 0;

  //retrieve the poll answer by reading the zigbee
  strcpy(messageToDecode, readZB(s_d));
  if (readCounter == 0) {
     if(diagNose) ws.textAll("no answer "); 
      return 50; //no answer
    }
      
    char *tail;

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
    // in tail at offset 14, 2 bytes with signalQuality reside   

    //sigQ = roundoff( (float) (extractValue(14, 2, 1, 0, tail) * 100 / 254 ), 1);
    //dtostrf((float)(extractValue(14, 2, 1, 0, tail) * 100 / 255 ), 0, 1, Inv_Data[which].sigQ);
    Inv_Data[which].sigQ = (extractValue(14, 2, 1, 0, tail) * 100 / 255 );
    if(diagNose) ws.textAll( "extracted sigQ = " + String(Inv_Data[which].sigQ) );
    //    dtostrf((float)(extractValue(68, 4, 1, 0, s_d) / 3.8 ), 0, 1, Inv_Data[which].acv);
    //    ws.textAll( "extracted ACV = " + String(Inv_Data[which].acv) );
// a YC600 message
// tail 06 01 3A 10 14 14 00 71 00 B5 7C FA 00 00 5E | 40 80 00 15 82 15 | FB FB 51 | B1 03 D4 0F 41 17 00 00 74 CF 00 | 00 00 76 70 | 6A 73 D0 6B 04 96 |
//      0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 | 15 16 17 18 19 20 | 21 22 23 | 24 25 26 27 28 29 30 31 32 33 34 | 35 36 37 38 | 39 40 41 42 43 44 |
// np                                                   0  1  2  3  4  5 | 6  7  8  | 9  10 11 12 13 14 15 16 17 18 19 | 20 21 22 23 | 24 25 26 27 28 29 |
//                                                          serial                                                      heath           frequency

// 00 00 | 00 | 00 00 | 00 00 01 | 72 | 07 2D | 88 01 78 62 | E8 20 1F 00 03 05 55 07 3F 03 03 03 01 00 | 00 01 00 
// 45 46 | 47 | 48 49 | 50 51 52 | 53 | 54 55 | 56 57 58 59 | 60 61 62 63 64 65 66 67 68 69 70 71 72 73 | 74 75 76 77 78 79 |
// 30 31 | 32 | 33 34 | 35 36 37 | 38 | 39 40 | 41 42 43 44 | 45 46 47 48 49 50 51 52 53 54 55 56 57 58 | 59
//         C0 |  DC1  |          | C1 |  DC2  |     ACV                                                 | EN0

//000000000000000000000000000000000000000000000000000000000000FEFE3A100E76",300);
//   break;


// a DS3 message
//FE0164010064FE034480001401D2FE0345C43A1000A8FE724481000006013A101414007100B57CFA00005E703000021300fbfb5cbbbb20000200e6ffff000000000000000006f506f9002e00340360138a17a70024001fffff054206900016f62b0018e451ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff3969fefe",300);
// tail starts with serial
// tail  70 30 00 02 13 00 | fb fb 5c bb bb 20 00 02 00   e6 ff ff 00 00   00 00 00 00 00 00 | 06 f5 | 06 f9 | 00 2e | 00 34 | 03 60 | 13 8a | 17 a7 00 24
//       0  1  2  3  4  5  | 6  7  8  9  10 11 12 13 14 | 15 16 17 18 19 | 20 21 22 23 24 25 | 26 27 | 28 29 | 30 31 | 32 33 | 34 35 | 36 37 | 38 39 40
//             serial                                                                          dcv2  |  dcv1 |  dcv3 |       |  ACV  | freq  | time

// | 00 1f ff ff 05 42 | 06  90 | 00 16 f6 2b | 00 18 e4 51 | ff ff ff
// | 42 43 44 45 46 47 | 48  49 | 50 51 52 53 | 54 55 56 57 | 50 59 60 61 62 | 63 64 | 65 66 67 68 | 69 70 71 72 | 73 74 75 
// |                   |  temp  |      en2    |       en1

// ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff3969fefe",300);

// new string
// 703000021300fbfb5cbbbb2000fc0001ffff000000000000000006e506ee015901da036e13882bbb01480026 ff ff 05 25 | 08 43 | 00 3a 40 b2 | 00 35 38 52 | 00 ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff3896fefe
        // attention s_d starts with the serial at offset 30 !!                             44            48temp  50   EN2       54     EN1
        //NEW VERSION of s_d generation with search string
        memset(&s_d[0], 0, sizeof(s_d)); //zero out all buffers we could work with "messageToDecode"
        delayMicroseconds(250);            //give memset a little bit of time to empty all the buffers
        strncpy(s_d, tail + 30, strlen(tail));
        delayMicroseconds(250); //give memset a little bit of time to empty all the buffers

   // #ifdef TEST
   //    ws.textAll("s_d =  " + String(s_d)); // show the tail
   // #endif

if( Inv_Prop[which].invType == 2 ) 
      { // is this a DS3, 
        if( diagNose != 0 ) consoleOut( "decoding a DS3 inverter");
        // ACV offset 34
        Inv_Data[which].acv = extractValue(68, 4, 1, 0, s_d) / 3.8 ;
        if( diagNose != 0 ) consoleOut( "extracted ACV = " + String(Inv_Data[which].acv) );
        
        // FREQ offset 36
        Inv_Data[which].freq = extractValue(72, 4, 1, 0, s_d)/100;
        if( diagNose != 0 ) consoleOut( "extracted FREQ = " + String(Inv_Data[which].freq) );
        
        // HEATH offset 48        
        Inv_Data[which].heath = extractValue(96, 4, 1, 0, s_d)*0.0198 - 23.84; //18
        if( diagNose != 0 ) consoleOut( "extracted HEATH = " + String(Inv_Data[which].heath) );
        
                // ******************  dc voltage   *****************************************
         //voltage ch1 offset 28
         Inv_Data[which].dcv[0] = extractValue( 52, 4, 1, 0, s_d ) * (float)1 / (float)48;
         // voltage ch2 offset 26
         Inv_Data[which].dcv[1] = extractValue( 56, 4, 1, 0, s_d ) * (float)1 / (float)48;
         // ******************  current   *****************************************
         //current ch1 offset 30
         Inv_Data[which].dcc[0] =  extractValue(60, 4, 1, 0, s_d ) * 0.0125;
         // current ch1 offset 34
         Inv_Data[which].dcc[1] =  extractValue(64, 4, 1, 0, s_d ) * 0.0125;
      } else {
         
        //yc600 or QS1 have these values in the same place
        //frequency ac voltage and temperature
        Inv_Data[which].acv = extractValue(56, 4, 1, 0, s_d) * ((float)1 / (float)1.3277) / 4 ;
        
        //frquency offset 48        
        Inv_Data[which].freq = 50000000 / extractValue(24, 6, 1, 0, s_d) ;
         
        // temperature offset 10
        Inv_Data[which].heath = extractValue(20, 4, 0.2752F, -258.7F, s_d);
      }
       if( Inv_Prop[which].invType == 0 ) { // is this a YC600
         // ******************  dc voltage   *****************************************
         //voltage ch1 offset 24
         Inv_Data[which].dcv[0] = (extractValue( 48, 2, (float)16, 0, s_d ) + extractValue(46, 1, 1, 0, s_d)) * (float)82.5 / (float)4096;

         //voltage ch2 offset 27
         Inv_Data[which].dcv[01] = (extractValue( 54, 2, (float)16, 0, s_d ) + extractValue(52, 1, 1, 0, s_d)) * (float)82.5 / (float)4096;

         // ******************  current   *****************************************
         //current ch1 offset 22
         Inv_Data[which].dcc[0] = (extractValue(47, 1, (float)256, 0, s_d) + extractValue(44, 2, 1, 0, s_d)) * (float)27.5 / (float)4096; //[A]

         //current ch1 offset 25
         Inv_Data[which].dcc[1] = (extractValue(53, 1, (float)256, 0, s_d) + extractValue(50, 2, 1, 0, s_d)) * (float)27.5 / (float)4096; 

       } 
        //********************************************************************************************
        //                                     SQ1
        //********************************************************************************************
        else if(Inv_Prop[which].invType == 1) //SQ1 inverter
        {
          //offset 21 -> byte for voltage ch3
          Inv_Data[which].dcv[1] = (extractValue( 48, 2, (float)16, 0, s_d ) + extractValue(40, 1, 1, 0, s_d)) * (float)82.5 / (float)4096;
          //offset 18 -> byte for voltage ch4
          Inv_Data[which].dcv[0] =  ( extractValue( 54, 2, (float)16, 0, s_d ) + extractValue(34, 1, 1, 0, s_d) ) * (float)82.5 / (float)4096;         // ***************************  current  *****************************************
          //current ch0 offset 22
          Inv_Data[which].dcc[1] = ( extractValue( 47, 1, (float)256, 0, s_d ) + extractValue(38, 2, 1, 0, s_d) ) * (float)27.5 / (float)4096; 
          //current ch1 offset 25
           Inv_Data[which].dcc[0] = ( extractValue( 53, 1, (float)256, 0, s_d ) + extractValue(32, 2, 1, 0, s_d)) * (float)27.5 / (float)4096;
           //offset 21 -> byte for voltage ch3
          Inv_Data[which].dcv[2] = (extractValue( 42, 2, (float)16, 0, s_d ) + extractValue(40, 1, 1, 0, s_d)) * (float)82.5 / (float)4096;
          //offset 18 -> byte for voltage ch4
          Inv_Data[which].dcv[3] =  ( extractValue( 36, 2, (float)16, 0, s_d ) + extractValue(34, 1, 1, 0, s_d) ) * (float)82.5 / (float)4096;         // ***************************  current  *****************************************
          //offset 19 and 20 for current and status ch3
          Inv_Data[which].dcc[2] = ( extractValue( 41, 1, (float)256, 0, s_d ) + extractValue(38, 2, 1, 0, s_d) ) * (float)27.5 / (float)4096; 
          //offset 16 and 17 for current and status ch4
           Inv_Data[which].dcc[3] = ( extractValue( 35, 1, (float)256, 0, s_d ) + extractValue(32, 2, 1, 0, s_d)) * (float)27.5 / (float)4096;
        }  
      
 
         yield();


/* 
we extract a value out of the inverter answer: en_extr
We have a value from the last poll: en_saved --> en_old
save the new enerrgy value en_extr to en_saved
We subtract these to get the increase en_saved -/- en_old
So en_saved is the value of total energy from the inverter
We keep stacking the increases so we have also en_inc_total
*/
// **********************************************************************
//               calculation of the power per panel
// **********************************************************************
    if(diagNose) ws.textAll("* * * * polled inverter " + String(which) + " * * * *");
    // 1st the time period 
    // at the start of this we have a value of the t_new[which] of the former poll
    // if this is 0 there was no former poll 
    switch (Inv_Prop[which].invType) {    
      case 0: // yc600
         t_extr = extractValue(34, 4, 1, 0, s_d); // dataframe timestamp
         break;
      case 1: // qs1
         t_extr = extractValue(60, 4, 1, 0, s_d); // dataframe timestamp
         break;
       case 2: //ds3 offset 38
         t_extr = (int)extractValue(76, 4, 1, 0, s_d); // dataframe timestamp ds3
         break;
    }
    
    //if the inverter had a reset, time new would be smaller than time old
    //t_saved is globally defined so we remember the last. With the new we can calculate the timeperiod
    if (t_extr < t_saved[which] || t_saved[which] == 0) { // there has been a reset 
    ts = t_extr;
    resetFlag = true;
    } else {
    ts = t_extr - t_saved[which]; // this is the timespan we use to calculate the power
    }  
    //whatever happened we remember t_ext as the new time value
    t_saved[which] = t_extr;

    if(diagNose) { 
      delay(100); 
      ws.textAll("extracted time = " + String(t_extr) + "  the timespan = " + String(ts));
    }

    // now for each channel 
    int increment = 10; // offset to the next energy value
    int btc = 6; // amount of bytes
    int offst = 74; // this is incremented with 10
    if(Inv_Prop[which].invType == 2) { offst = 100; increment = 8; btc = 8; } // for the DS3 we have different offset/increment
    
    //float total = 0;
    float en_extr = 0;
    float en_incr = 0;
    float en_incr_total = 0;
    float power = 0;
    // for every panel of inverter which we go through this loop


    for(int x = 0; x < 4; x++ ) 
    {
         int offset = 0;     
    
         if(Inv_Prop[which].conPanels[x]) { // is this panel connected ? otherwise skip
            // first store the last value of energy_new temporary in energy_old
            // after the calulation we dont need it anymore
            en_old[x] = en_saved[which][x]; // en_new (per inverter per panel needs to be global
            
            if(diagNose) {delay(100); ws.textAll(" * decoding panel " + String(x) + " \n en_old " + String(en_old[x]) );}

            if ( Inv_Prop[which].invType == 0) { // for yc600 channels 1 and 2 are swapped compared to qs1
                if(x==0)  offset = offst + 8;  else offset = offst;
            // now we extract a new energy_new[which][x] 
            en_extr = extractValue(offset, btc, 1, 0, s_d); // offset 74 todays module energy channel 0
            } else {
            // for ds1 and qs10 now we extract a new energy_new[which][x] 
            en_extr = extractValue(offst+x*increment, btc, 1, 0, s_d); // offset 74 todays module energy channel 0
            }// we calculate a new energy value for this panel and remember it
            if ( Inv_Prop[which].invType == 2) {
              en_saved[which][x] = (en_extr / (float)1000 /100) * 1.66; //[Wh]
            } else {
              en_saved[which][x] = (en_extr * 8.311F / (float)3600); //[Wh]
            }
           
            if(diagNose) {delay(100); ws.textAll("en_extr " + String(en_extr) + "  en_saved " + String(en_saved[which][x]) + "\n\n" );}

            // calculate the energy increase with or without reset and totalize it
            if(resetFlag){
            en_incr = en_saved[which][x]; // the inverter energy value per panel  
            } else {
            en_incr = en_saved[which][x] - en_old[x]; //increase is new -/- old
            }

            en_incr_total += en_incr; //totalize the energy increase for this poll
            //add en_incr to Inv_Data[which].en_total 
            //Inv_Data[which].en_total += en_incr; // stack the increase
            
            //calculate the power for this panel and remember
             power = en_incr / ts * (float)3600; //[W]
//            if ( Inv_Prop[which].invType == 2) {
//              power = en_incr / ts * (float)3600; //[W]
//            } else {
//              power = en_incr / ts * (float)3600; //[W]
//            }
            Inv_Data[which].power[x] = round1(power);
            total_pwr += power;
            
            yield();
            if(diagNose) 
            {
                if(resetFlag) { delay(100); ws.textAll("1st poll after inverter (re)start"); }
                delay(100);
                ws.textAll("en_incr " + String(en_incr) + "  power " + String(power) );
            }     

        } else {
          if(diagNose) {delay(100); ws.textAll("no panel " + String(x));}
        }
    }           
     // now we extracted all values per panel and added the increased energy
     // we have invData[which].power[x] and invData[which].en_total
    Inv_Data[which].en_total += en_incr_total; // stack the increase
    //dtostrf( total_pwr, 0, 1, Inv_Data[which].power[4]); // write the total value in the struct   
    Inv_Data[which].pw_total = total_pwr;

     yield();
     if(diagNose){
     delay(100);
     ws.textAll("total energy increase: " + String(en_incr_total) + "\n");
     delay(100);
     ws.textAll("total energy stacked in Inv_Data[which].en_total: " + String(Inv_Data[which].en_total) + "\n");
     }

    return 0;
}       

      

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
    char tempMsgBuffer[64] = {0}; // was 254
    yield();

    strncpy(tempMsgBuffer, toDecode + startPosition, valueLength);
    
    // now we have the part of the string "startposition - number of bytes"
    // we calculate the value it is representing with strtol and correct it with valueSlope and offset
    yield();

    return (valueSlope * (float)strtol(tempMsgBuffer, 0, 16)) + valueOffset;
}

//// *******************************************************************************************************************
////                             extract values
//// *******************************************************************************************************************
//float extractValue(uint8_t startPosition, uint8_t valueLength, float valueSlope, float valueOffset, char toDecode[CC2530_MAX_SERIAL_BUFFER_SIZE])
//{
////    Serial.println("re declare tempMsgBuffer");
//char tempMsgBuffer[64] = {0}; // was 254
//yield();
//    
//    memset(&tempMsgBuffer[0], 0, sizeof(tempMsgBuffer)); //zero out all buffers we could work with "messageToDecode"
//    delayMicroseconds(250);                              //give memset a little bit of time to empty all the buffers
//    strncpy(tempMsgBuffer, toDecode + startPosition, valueLength);
//    delayMicroseconds(250); //give memset a little bit of time to empty all the buffers
//
//    yield();
//    return (valueSlope * (float)StrToHex(tempMsgBuffer)) + valueOffset;
//}

// ************************************************************************************
//                mqtt send polled data
// ************************************************************************************
void mqttPoll(int which) {

if(Mqtt_Format == 0) return;  

  char Mqtt_send[26]={0};  
  strcpy(Mqtt_send, Mqtt_outTopic);
  if( Mqtt_send[strlen(Mqtt_send)-1] == '/' ) {
    strcat(Mqtt_send, String(Inv_Prop[which].invIdx).c_str());
  }
  bool reTain = false;
  char pan[50]={0};
  char tail[40]={0};
  char toMQTT[512]={0};

// the json to domoticz must be something like {"idx" : 7, "nvalue" : 0,"svalue" : "90;2975.00"}
 
   switch( Mqtt_Format)  { 
    case 1: 
       snprintf(toMQTT, sizeof(toMQTT), "{\"idx\":%d,\"nvalue\":0,\"svalue\":\"%.1f;%.2f\"}" , Inv_Prop[which].invIdx , Inv_Data[which].pw_total, Inv_Data[which].en_total);
       break;
       // length 46

     case 2: // for not domoticz we have a different mqtt string how does this look?
       snprintf(toMQTT, sizeof(toMQTT), "{\"inv\":\"%d\",\"temp\":\"%.1f\",\"p0\":\"%.1f\",\"p1\":\"%.1f\",\"p2\":\"%.1f\",\"p3\":\"%.1f\",\"energy\":\"%.2f\"}" ,which, Inv_Data[which].heath, Inv_Data[which].power[0],Inv_Data[which].power[1], Inv_Data[which].power[2], Inv_Data[which].power[3], Inv_Data[which].en_total);
       break;  
       
     case 3:
        snprintf(toMQTT, sizeof(toMQTT), "{\"inv_serial\":\"%s\",\"freq\":%.1f,\"temp\":%.1f,\"acv\":%.1f" , Inv_Prop[which].invSerial, Inv_Data[which].freq, Inv_Data[which].heath, Inv_Data[which].acv);
        //char pan[50]={0};
        if( Inv_Prop[which].invType == 1 ) { // qs1
            sprintf(pan, ",\"dcv\":[%.1f,%.1f,%.1f,%.1f]", Inv_Data[which].dcv[0], Inv_Data[which].dcv[1],Inv_Data[which].dcv[2],Inv_Data[which].dcv[3]);
            strcat(toMQTT, pan);
            sprintf(pan, ",\"dcc\":[%.1f,%.1f,%.1f,%.1f]", Inv_Data[which].dcc[0], Inv_Data[which].dcc[1],Inv_Data[which].dcc[2],Inv_Data[which].dcc[3]);
            strcat(toMQTT, pan);
            sprintf(pan, ",\"pwr\":[%.1f,%.1f,%.1f,%.1f]", Inv_Data[which].power[0], Inv_Data[which].power[1],Inv_Data[which].power[2],Inv_Data[which].power[3]);
            strcat(toMQTT, pan);
            sprintf(pan, ",\"en\":[%.2f,%.2f,%.2f,%.2f]}", en_saved[which][0], en_saved[which][1], en_saved[which][2], en_saved[which][3]);
            strcat(toMQTT, pan);
        } else {
            sprintf(pan, ",\"dcv\":[%.1f,%.1f]", Inv_Data[which].dcv[0], Inv_Data[which].dcv[1]);
            strcat(toMQTT, pan);
            sprintf(pan, ",\"dcc\":[%.1f,%.1f]", Inv_Data[which].dcc[0], Inv_Data[which].dcc[1]);
            strcat(toMQTT, pan);
            sprintf(pan, ",\"pwr\":[%.1f,%.1f]", Inv_Data[which].power[0], Inv_Data[which].power[1]);
            strcat(toMQTT, pan);
            sprintf(pan, ",\"en\":[%.2f,%.2f]}", en_saved[which][0], en_saved[which][1]);          
            strcat(toMQTT, pan);
        }
        reTain=true;
        break;
//
    case 4:

        snprintf(toMQTT, sizeof(toMQTT), "{\"inv_serial\":\"%s\",\"freq\":%.1f,\"temp\":%.1f,\"acv\":%.1f" , Inv_Prop[which].invSerial, Inv_Data[which].freq, Inv_Data[which].heath, Inv_Data[which].acv);      
        sprintf(pan, ",\"ch0\":[%.1f,%.1f,%.1f,%.2f]", Inv_Data[which].dcv[0], Inv_Data[which].dcc[0], Inv_Data[which].power[0], en_saved[which][0]);  
        strcat(toMQTT, pan);
        sprintf(pan, ",\"ch1\":[%.1f,%.1f,%.1f,%.2f]", Inv_Data[which].dcv[1], Inv_Data[which].dcc[1], Inv_Data[which].power[1], en_saved[which][1]);  
        strcat(toMQTT, pan);

        if( Inv_Prop[which].invType == 1 ) { // add ch2 and ch3
            sprintf(pan, ",\"ch2\":[%.1f,%.1f,%.1f,%.2f]", Inv_Data[which].dcv[2], Inv_Data[which].dcc[2], Inv_Data[which].power[2], en_saved[which][2]);  
            strcat(toMQTT, pan);
            sprintf(pan, ",\"ch3\":[%.1f,%.1f,%.1f,%.2f]", Inv_Data[which].dcv[3], Inv_Data[which].dcc[3], Inv_Data[which].power[3], en_saved[which][3]);  
            strcat(toMQTT, pan);
        }
        sprintf(tail, ",\"totals\":[%.1f,%.2f]}", Inv_Data[which].pw_total, Inv_Data[which].en_total);
        strcat(toMQTT, tail);
        reTain=true;
        break;
     
     case 5: // for thingspeak we have a different format
       snprintf(toMQTT, sizeof(toMQTT), "field1=%d&field2=%.1f&field3=%.1f&field4=%.1f&field5=%.1f&field6=%.1f&field7=%.2f&field8=%.1f&status=MQTTPUBLISH" ,which, Inv_Data[which].heath, Inv_Data[which].power[0],Inv_Data[which].power[1], Inv_Data[which].power[2], Inv_Data[which].power[3], Inv_Data[which].en_total, Inv_Data[which].acv);
       reTain=false;
       break;
    }


   // mqttConnect() checks first if we are connected, if not we connect anyway
   if(mqttConnect() ) MQTT_Client.publish ( Mqtt_send, toMQTT, reTain );
 }


//// ************************************************************************************
////                mqtt send polled data
//// ************************************************************************************
//void mqttPoll(int which) {
//
//if(Mqtt_Format == 0) return;  
//
//  char Mqtt_send[26]={0};  
//  strcpy(Mqtt_send, Mqtt_outTopic);
//  if( Mqtt_send[strlen(Mqtt_send)-1] == '/' ) {
//    strcat(Mqtt_send, String(Inv_Prop[which].invIdx).c_str());
//  }
//
//  StaticJsonDocument doc(1024);
//  JsonObject root = doc.to<JsonObject>();
//// the json to domoticz must be something like {"idx" : 7, "nvalue" : 0,"svalue" : "90;2975.00"}
// 
//   switch( Mqtt_Format)  { 
//    case 1: 
//       root["idx"] = Inv_Prop[which].invIdx;
//       root["nvalue"] = 0;
//       root["svalue"][0] = Inv_Data[which].pw_total;
//       
//       snprintf(toMQTT, sizeof(toMQTT), "{\"idx\":%d,\"nvalue\":0,\"svalue\":\"%.1f;%.2f\"}" , Inv_Prop[which].invIdx , Inv_Data[which].pw_total, Inv_Data[which].en_total);
//       break;
//       // length 46
//
//     case 2: // for not domoticz we have a different mqtt string how does this look?
//       snprintf(toMQTT, sizeof(toMQTT), "{\"inv\":\"%d\",\"temp\":\"%.1f\",\"p0\":\"%.1f\",\"p1\":\"%.1f\",\"p2\":\"%.1f\",\"p3\":\"%.1f\",\"energy\":\"%.2f\"}" ,which, Inv_Data[which].heath, Inv_Data[which].power[0],Inv_Data[which].power[1], Inv_Data[which].power[2], Inv_Data[which].power[3], Inv_Data[which].en_total);
//       break;  
//       
//     case 3:
//        snprintf(toMQTT, sizeof(toMQTT), "{\"inv_serial\":\"%s\",\"freq\":%.1f,\"temp\":%.1f,\"acv\":%.1f" , Inv_Prop[which].invSerial, Inv_Data[which].freq, Inv_Data[which].heath, Inv_Data[which].acv);
//        //char pan[50]={0};
//        if( Inv_Prop[which].invType == 1 ) { // qs1
//            sprintf(pan, ",\"dcv\":[%.1f,%.1f,%.1f,%.1f]", Inv_Data[which].dcv[0], Inv_Data[which].dcv[1],Inv_Data[which].dcv[2],Inv_Data[which].dcv[3]);
//            strcat(toMQTT, pan);
//            sprintf(pan, ",\"dcc\":[%.1f,%.1f,%.1f,%.1f]", Inv_Data[which].dcc[0], Inv_Data[which].dcc[1],Inv_Data[which].dcc[2],Inv_Data[which].dcc[3]);
//            strcat(toMQTT, pan);
//            sprintf(pan, ",\"pwr\":[%.1f,%.1f,%.1f,%.1f]", Inv_Data[which].power[0], Inv_Data[which].power[1],Inv_Data[which].power[2],Inv_Data[which].power[3]);
//            strcat(toMQTT, pan);
//            sprintf(pan, ",\"en\":[%.2f,%.2f,%.2f,%.2f]}", en_saved[which][0], en_saved[which][1], en_saved[which][2], en_saved[which][3]);
//            strcat(toMQTT, pan);
//        } else {
//            sprintf(pan, ",\"dcv\":[%.1f,%.1f]", Inv_Data[which].dcv[0], Inv_Data[which].dcv[1]);
//            strcat(toMQTT, pan);
//            sprintf(pan, ",\"dcc\":[%.1f,%.1f]", Inv_Data[which].dcc[0], Inv_Data[which].dcc[1]);
//            strcat(toMQTT, pan);
//            sprintf(pan, ",\"pwr\":[%.1f,%.1f]", Inv_Data[which].power[0], Inv_Data[which].power[1]);
//            strcat(toMQTT, pan);
//            sprintf(pan, ",\"en\":[%.2f,%.2f]}", en_saved[which][0], en_saved[which][1]);          
//            strcat(toMQTT, pan);
//        }
//        reTain=true;
//        break;
////
//    case 4:
//
//        snprintf(toMQTT, sizeof(toMQTT), "{\"inv_serial\":\"%s\",\"freq\":%.1f,\"temp\":%.1f,\"acv\":%.1f" , Inv_Prop[which].invSerial, Inv_Data[which].freq, Inv_Data[which].heath, Inv_Data[which].acv);      
//        sprintf(pan, ",\"ch0\":[%.1f,%.1f,%.1f,%.2f]", Inv_Data[which].dcv[0], Inv_Data[which].dcc[0], Inv_Data[which].power[0], en_saved[which][0]);  
//        strcat(toMQTT, pan);
//        sprintf(pan, ",\"ch1\":[%.1f,%.1f,%.1f,%.2f]", Inv_Data[which].dcv[1], Inv_Data[which].dcc[1], Inv_Data[which].power[1], en_saved[which][1]);  
//        strcat(toMQTT, pan);
//
//        if( Inv_Prop[which].invType == 1 ) { // add ch2 and ch3
//            sprintf(pan, ",\"ch2\":[%.1f,%.1f,%.1f,%.2f]", Inv_Data[which].dcv[2], Inv_Data[which].dcc[2], Inv_Data[which].power[2], en_saved[which][2]);  
//            strcat(toMQTT, pan);
//            sprintf(pan, ",\"ch3\":[%.1f,%.1f,%.1f,%.2f]", Inv_Data[which].dcv[3], Inv_Data[which].dcc[3], Inv_Data[which].power[3], en_saved[which][3]);  
//            strcat(toMQTT, pan);
//        }
//        sprintf(tail, ",\"totals\":[%.1f,%.2f]}", Inv_Data[which].pw_total, Inv_Data[which].en_total);
//        strcat(toMQTT, tail);
//        reTain=true;
//        break;
//     
//     case 5: // for thingspeak we have a different format
//       snprintf(toMQTT, sizeof(toMQTT), "field1=%d&field2=%.1f&field3=%.1f&field4=%.1f&field5=%.1f&field6=%.1f&field7=%.2f&status=MQTTPUBLISH" ,which, Inv_Data[which].heath, Inv_Data[which].power[0],Inv_Data[which].power[1], Inv_Data[which].power[2], Inv_Data[which].power[3], Inv_Data[which].en_total);
//       reTain=false;
//       break;
//    }
//
//
//   // mqttConnect() checks first if we are connected, if not we connect anyway
//   if(mqttConnect() ) MQTT_Client.publish ( Mqtt_send, toMQTT, reTain );
// }
