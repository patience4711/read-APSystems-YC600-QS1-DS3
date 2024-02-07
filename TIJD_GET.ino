void getTijd() {

  timeRetrieved = false; // stays false until time is retrieved  
  timeClient.begin();
  //unsigned long epochTime = 0;
  //get the time, if fails we try again during healthcheck

  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();


  //Serial.print("Epoch Time: ");
  //Serial.println(epochTime);

    // now convert NTP time into unix tijd:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    //const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
//    unsigned long epoch = secsSince1900 - seventyYears + atof(gmtOffset) * 60; // * 60 weggehaald omdat gmtOffset in minuten is
//    unsigned long epochTime = timeClient.getEpochTime;
    // we have to do this conditional, if time retrieving failed
    if (epochTime < 1000) {
    ntpUDP.stop();
    return;
  } else {
   
    epochTime += atoi(gmtOffset) * 60;
    setTime(epochTime); // dit moeten we doen omdat anders zomertijd() niet werkt
    //Serial.print("epoch gecorrigeerd voor gmtOffset = "); Serial.println(epochTime);
    if ( zomerTijd == true ) {
    //Serial.print("zomerTijd[0] = een o dus on "); Serial.println(String(zomerTijd));  
      if (zomertijd() == true) {  
        epochTime += 3600; // een uur erbij
        setTime(epochTime);
        //DebugPrint("epoch corrected with dts = "); //DebugPrintln(epochTime);
      }
    }
    timeRetrieved=true;  
    #ifdef LOG 
    Update_Log(1, "got time"); 
    #endif
    }
    //DebugPrint(" Unix time epoch = ");
    //DebugPrintln(epochTime);
  
ntpUDP.stop();
//
//  // de tijd is nu opgehaald en in setTime gestopt
//  // dus met de tijden die met setTime zijn opgeslagen gaan we  alle berekeningen doen
//  
//DebugPrint("het uur is ");  //DebugPrint(hour());
//DebugPrint("   aantal minuten "); //DebugPrintln(minute());
datum = day();
//
//yield();
delay(10);
sun_setrise(); //to calulate moonshape sunrise etc. and the switchtimes

//  switchonTime = sunrise - 900;
//  switchoffTime = sunset + 900; // nightmode starts at 15 min after sunset
}
