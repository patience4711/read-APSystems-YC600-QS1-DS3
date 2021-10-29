void getTijd() {

  timeRetrieved = false; // stays false until time is retrieved  
  timeClient.begin();

  //get the time, if fails we try again during healthcheck

  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();

    // we have to do this conditional, if time retrieving failed
    if (epochTime < 1000) {
    ntpUDP.stop();
    return;
  } else {
   
    epochTime += atoi(timezone) * 60;
    setTime(epochTime); // we have to do this 1st otherwise the calculation zomertijd() would fail
    if ( zomerTijd == true ) {
      if (zomertijd() == true) {  
        epochTime += 3600; // add one hour
        setTime(epochTime);
        //DebugPrint("epoch corrected with dts = "); //DebugPrintln(epochTime);
      }
    }
    timeRetrieved=true;  
    Update_Log("system", "time retrieved");
    }
  
ntpUDP.stop();

datum = day();
//
//yield();
delay(10);
sun_setrise(); //to calulate moonshape sunrise etc. and the switchtimes

}

bool zomertijd(){
// are we between the last sunday in okt and the last sunday in march than wintertime

int Sunmarch = 7 - dow(year(), 3 ,1) + 1;
#ifdef DEBUG
Serial.println("the first sunday of march is date " + String(Sunmarch));
#endif
// add a week as long as smaller than 25 (the last is max 31) 
while(Sunmarch < 25){
  Sunmarch = Sunmarch + 7;
}

#ifdef DEBUG
Serial.print( "the last sunday of march is date " + String(Sunmarch) );
#endif

// say dow(2021,10,1) = daynr 5 = fry (sunday=0)
// the date of the 1st sunday in oct = sunokt = 7 - 5 + 1 = 3 oct
// the while counts
//10
//17
//24 
//31
//so last sunday = 31 oct

// dow goes from 0 to 6, sunday is 0
int Sunoct = 7 - dow(year(), 10 ,1) + 1;
#ifdef DEBUG
Serial.println("the first sunday in oct is date " + String(Sunoct));
#endif
// the last is max 31 
while(Sunoct < 25){
  Sunoct = Sunoct + 7;
}
// now we have the last sunday in oct
#ifdef DEBUG
Serial.print( "the last sunday of oct is date " + String(Sunoct) );
#endif

if(((month() == 3 and day() >= Sunmarch) or month() > 3) and ((month() == 10 and day() < Sunoct) or month() < 10)) {  
DebugPrintln("it is summertime");
return true;
} else {
DebugPrintln("it is not summertime");
return false; 
}
}

int dow(int y, int m, int d) // returns what daynr for a specific date
{
static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
y -= m < 3;
return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}
