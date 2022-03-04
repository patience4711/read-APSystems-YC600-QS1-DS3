// ******************************************************************
//                    decode polling answer
// ******************************************************************
int decodePollAnswer(int which)
{
//Serial.println("1 decodePollAnswer which = " + String(which) );
// we need the fullincomingMessge and the inverterSerial
  char messageToDecode[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};

  char s_d[254] = {0};
  uint8_t Message_begin_offset = 0;    

  int t_old = 0;
 
  float en_old[4] = {0}; // energy old
  float energy_old_total = 0;
  int t_extr  = 0;
  int ts = 0;
  bool resetFlag = false;
   
if(!*inMessage) return 50; // if empty we return with erroCode 50

    strncpy(messageToDecode, inMessage, strlen(inMessage));
    //delayMicroseconds(250); //give memset a little bit of time to empty all the buffers
    
    // we dont need this char anymore so make it empty
    memset( &inMessage, '\0', sizeof(inMessage) ); //zero out the 
    delayMicroseconds(250);
    
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
    ws.textAll("* * * * polled inverter " + String(which) + " * * * *");
    #ifdef TEST
    ws.textAll("testCount " + String(testCount));
    #endif
    // 1st the time period 
    // at the start of this we have a value of the t_new[which] of the former poll
    // if this is 0 there was no former poll 
          
    if(Inv_Prop[which].invType == 1 )
    {
    t_extr = extractValue(60, 4, 1, 0, s_d); // dataframe timestamp
    } else {
    t_extr = extractValue(34, 4, 1, 0, s_d); // dataframe timestamp
    }
    #ifdef TEST
    t_extr += testCount*300;
    #endif  

    //if the inverter had a reset, time new would be smaller than time old
    //t_saved is globally defined so we remember the last. With the new we can calculate the timeperiod
    if (t_extr < t_saved[which] || t_saved[which] == 0){ // there has been a reset 
    ts = t_extr;
    resetFlag = true;
    } else {
    ts = t_extr - t_saved[which]; // this is the timespan we use to calculate the power
    }  
    //whatever happened we remember t_ext as the new time value
    t_saved[which] = t_extr;

    if(diagNose) ws.textAll("extracted time = " + String(t_extr) + "  the timespan = " + String(ts));

    // now for each channel 
    
    int offst = 74; // this is incremented with 10
    //float total = 0;
    float en_extr = 0;
    float en_incr = 0;
    float en_incr_total = 0;
    float power = 0;
    // for every panel of inverter which we go through this loop


    for(int x = 0; x < 4; x++ ) 
    {   
         if(Inv_Prop[which].conPanels[x]) { // is this panel connected ? otherwise skip
         #ifndef TEST 
         if(diagNose) {delay(100); ws.textAll(" * * * decoding panel " + String(x) + " * * * ");}           
         #endif
            // first store the last value of energy_new temporary in energy_old
            // after the calulation we dont need it anymore
            en_old[x] = en_saved[which][x]; // en_new (per inverter per panel needs to be global
#ifdef TEST            
            if(diagNose) {delay(100); ws.textAll(" * decoding panel " + String(x) + " * en_old " + String(en_old[x]) );}
#endif
            // now we extract a new energy_new[which][x] 
            en_extr = extractValue(offst+x*10, 6, 1, 0, s_d); // offset 74 todays module energy channel 0
#ifdef TEST
            en_extr += testCount*500;
#endif
            //we calculate a new energy value for this panel and remember it
            en_saved[which][x] = (en_extr * 8.311F / (float)3600); //[Wh]
#ifdef TEST            
            if(diagNose) {delay(100); ws.textAll("en_extr " + String(en_extr) + "  en_saved " + String(en_saved[which][x]) );}
#endif            

            // calculate the energy increase with or without reset and totalize it
            if(resetFlag){
            en_incr = en_saved[which][x]; // the increase is the new value 
            } else {
            en_incr = en_saved[which][x] - en_old[x]; //increase is new-old
            }

            en_incr_total += en_incr; //totalize the energy increase for this poll
            //add en_incr to Inv_Data[which].en_total 
            //Inv_Data[which].en_total += en_incr; // stack the increase
            
            //calculate the power for this panel and remember
            power = en_incr / ts * (float)3600; //[W]
            dtostrf( power, 0, 1, Inv_Data[which].power[x]); // write the value in the struct
            
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

#ifdef TEST
     yield();
     if(diagNose){
     delay(100);
     ws.textAll("total energy increase: " + String(en_incr_total) + "\n");
     delay(100);
     ws.textAll("total energy stacked in Inv_Data[which].en_total: " + String(Inv_Data[which].en_total) + "\n");
     }
#endif
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
       // en_total is the stacked energy increase
       String sValue="\"svalue\":\""; // "svalue":"
       sValue += String(Inv_Data[which].power[4]) + ";" + String(Inv_Data[which].en_total, 2) ; //total of the 2 or 4
       toMQTT += ",\"idx\":" + String(Inv_Prop[which].invIdx) + ",\"nvalue\":0," + sValue + "\"}";
       //Serial.println("we publish to domoticz, mess is : " + toMQTT);
    }  else { 
        // for not domoticz we have a different mqtt string how does this look?
       toMQTT += ",\"temp\":\"" + String(Inv_Data[which].heath) + "\",\"p0\":\"" + String(Inv_Data[which].power[0]) + "\",\"p1\":\"" + String(Inv_Data[which].power[1]) + "\"";
       if( Inv_Prop[which].invType == 1 ) {
       toMQTT += ",\"p2\":\"" + String(Inv_Data[which].power[2]) + "\",\"p3\":\"" + String(Inv_Data[which].power[3])  +  "\"";
       }
       toMQTT += ",\"energy\":\"" + String(Inv_Data[which].en_total, 1) + "\"";
       //Serial.println("we do not publish to domoticz, mess is : " + toMQTT);    
       toMQTT += "}";  
       }
   //DebugPrintln("mqtt mess :"); //DebugPrintln(toMQTT);
   MQTT_Client.publish ( Mqtt_outTopic.c_str(), toMQTT.c_str() );
 }

 
