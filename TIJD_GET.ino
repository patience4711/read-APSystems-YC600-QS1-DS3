void getTijd(){
  timeClient.begin();
  timeClient.update();

  unsigned long epochTime = timeClient.getEpochTime();
  DebugPrint("Epoch Time: ");
  DebugPrintln(epochTime);

    // now convert NTP time into unix tijd:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    //const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
//    unsigned long epoch = secsSince1900 - seventyYears + atof(timezone) * 60; // * 60 weggehaald omdat timezone in minuten is
//    unsigned long epochTime = timeClient.getEpochTime;
    // dit moeten we conditioneel doen, als het tijdophalen mislukt
    if (epochTime < 1000) {
    //foutvlag = 1; // als we foutvlag zetten hoe wordt die dan nul?
    ntpUDP.stop();
    timeRetrieved=false;
    
  } else {
 
    epochTime += atoi(timezone) * 60;
    setTime(epochTime); // dit moeten we doen omdat anders zomertijd() niet werkt
    if ( zomerTijd ) {
      if (zomertijd() == true) {  
        epochTime += 3600; // een uur erbij
        setTime(epochTime);
        DebugPrint("epoch gecorrigeerd voor zomertijd = "); DebugPrintln(epochTime);
      }
    }

    timeRetrieved=true;  
    
    DebugPrint(" Amsterdam Unix time epoch = ");
    DebugPrintln(epochTime);
  
    ntpUDP.stop();
    }
    //
    // the time is yes or no retrieved and put in setTime ( can be old )
    
    datum = day();
    //
    sun_setrise(); //to calulate moonshape sunrise etc.
    // calculate the unixtijd at start of this day
    tmElements_t myElements = {0, 0, 0, 0, day(), month(), (year()-1970)};
    dagbegintijd = makeTime(myElements);
    // arm the timers for recalculation
    for(int i=0; i<TIMERCOUNT; i++) {
      if(timerActive[i] == '1') mustCalc[i] = true; // all active timers armed for recalculation
    }
}

bool zomertijd(){
// are we between the last sunday in okt and the last sunday in march than wintertime
#ifdef DEBUG
Serial.println(" de datum is " + String(day()) + String(month())+ String(year()) );
#endif
//Serial.print("the first day of mach ");
//int fstDaymarc = dow(year(), 10 ,1);
//int Sunmarch ;
//Serial.println("de eerste dag van march = dagnumber " + String(fstDaymarch));
int Sunmarch = 7 - dow(year(), 3 ,1) + 1;
#ifdef DEBUG
Serial.println("the first sunday of march is date " + String(Sunmarch));
#endif
// the last is max 31 
while(Sunmarch < 25){
  Sunmarch = Sunmarch + 7;
}

while(Sunmarch <= 24){
  Sunmarch = Sunmarch + 7;
}
#ifdef DEBUG
//Serial.print( "the last sunday of march is date " + String(Sunmarch) );
#endif
//int fstDayoct = dow(year(), 10 ,1);
//int Sunoct ;
//Serial.println("de eerste dag van oct = dagnumber " + String(fstDayoct));
//say this is daynumber 4 (thu), we know that su = day 0
//than we know that the next sunday = overflow of 6 over 3 days (7-4) offset to Sunday = 7 - eersteoct
// the date of fst sunday is then 1 + offset = 4 okt

// dow gaat van 0 naar 6, zondag is 0
int Sunoct = 7 - dow(year(), 10 ,1) + 1;
#ifdef DEBUG
//Serial.println("the first sunday in oct is date " + String(Sunoct));
#endif
// the last is max 31 
while(Sunoct < 25){
  Sunoct = Sunoct + 7;
}
#ifdef DEBUG
//Serial.println("the last sunday of october is date " + String(Sunoct));
#endif

if(((month() == 3 and day() >= Sunmarch) or month() > 3) and ((month() == 10 and day() < Sunoct) or month() < 10)) {  
//DebugPrintln("it is summertime");
return true;
} else {
//DebugPrintln("it is not summertime");
return false; 
}
}

int dow(int y, int m, int d) // returns what daynr for a specific date
{
static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
y -= m < 3;
return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}
