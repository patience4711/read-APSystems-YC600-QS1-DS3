// deze functie berekent de zonsopkomst en zonsondergangstijden
void sun_setrise() {

//float OUR_longtitude = atof(lengte);
//float OUR_latitude = atof(breedte);
float OUR_longtitude = longi;
float OUR_latitude = lati;
float OUR_timezone = atof(gmtOffset); //   120                     // localtime with UTC difference in minutes

sunMoon  sm;

//  tmElements_t  tm;                             // specific time
//  tm.Second = 0;
//  tm.Minute = 12;
//  tm.Hour   = 12;
//  tm.Day    = 3;
//  tm.Month  = 8;
//  tm.Year   = 2016 - 1970;
//  time_t s_date = makeTime(tm);
//  Serial.println("RTC has set the system time");
  sm.init(OUR_timezone, OUR_latitude, OUR_longtitude);


//  uint32_t jDay = sm.julianDay();               // Optional call
//  mDay = sm.moonDay();
// if (mDay < 13){ maan = "cresc. moon";} //crescent moon"
// if (mDay > 15){maan = "waning moon";}
// if (mDay == 13 || mDay == 14 || mDay == 15){maan = "full moon";}
// if (mDay == 0 || mDay == 1 || mDay == 28){maan = "new moon";} 

      time_t sunrise = sm.sunRise();
      time_t sunset  = sm.sunSet();
      
      if ( zomerTijd ) { // er is zomertijd ingesteld
            if (zomertijd() == true) { // we kijken of het zomertijd is
              sunrise = sunrise + 3600; // seconden
              sunset  = sunset + 3600;
              dst = 1; //summer time
              } else {
              dst = 2; // winter time 
              }
       } else { 
           dst = 0; 
           }
   switchonTime = sunrise + pollOffset*60; // was -900
   switchoffTime = sunset - pollOffset*60; // was +900
 
 }


// ************************* deze functie bepaalt of zomertijd van toepassing is
bool zomertijd() {
    //DebugPrint(" the date is ");
    //DebugPrint(day());
    //DebugPrint(" ");
    //DebugPrint(month());
    //DebugPrint(" ");
    //DebugPrint(year());
    //DebugPrintln(" ");

    int eerstemrt = dow(year(), 3 ,1);
    int zdmrt;

    if (eerstemrt == 0) {
     zdmrt = 1;
    } else {
     zdmrt = 1 + (7 - eerstemrt);
    }

    while(zdmrt <= 24){
      zdmrt = zdmrt + 7;
    }
    //DebugPrint("the last sunday of march is ");
    //DebugPrint(zdmrt);
    //DebugPrintln("");
    
    //Serial.print("de eerste dag van oktober is dag ");
    int eersteoct = dow(year(), 10 ,1);
    int zdoct ;
    //Serial.print(eersteoct);
    //Serial.println("");
    // dow gaat van 0 naar 6, zondag is 0
    //Serial.print("de eerste zondag van zondag van oct is dag ");
    if (zdoct == 0) {
    zdoct = 1;
    } else {
    zdoct = 1+(7-eersteoct);
    }
    //Serial.print(zdoct);
    //Serial.println("");
    
    while(zdoct <= 24){
      zdoct = zdoct + 7;
    }
    //DebugPrint("de laatste zondag van october is dag ");
    //DebugPrint(zdoct);
    //DebugPrintln("");

    if(((month() == 3 and day() >= zdmrt) or month() > 3) and ((month() == 10 and day() < zdoct) or month() < 10)) {  
    //DebugPrintln("het is zomertijd");
    return true;
    } else {
    //DebugPrintln("het is geen zomertijd");
    return false; 
    }
}

int dow(int y, int m, int d) {
static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
y -= m < 3;
return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

void tijd_convert () {

}


// een universele funtie om het aantal uren en minuten uit de char in een string te zetten
// als wat true dan geeft ie de uren en anders de minuut
int tijd_cvrt(char TIJD[6], bool wat) {
     int uur = 0;
     for (int i = 0; i < 2; i++) {
          char c = TIJD[i];
               if (c < '0' || c > '9') break;
               uur *= 10;
               uur += (c - '0');
          }
     int minuut = 0;
     for (int i = 3; i < 5; i++) { // 2 = de :
          char c = TIJD[i];
               if (c < '0' || c > '9') break;
               minuut *= 10;
               minuut += (c - '0');
          }
if (wat) { return uur; } else { return minuut;}
}
