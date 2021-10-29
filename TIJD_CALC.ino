//// ********** bereken de schakeltijden voor deze dag at boot***************
//void switchCalc() {
//
//for(int i=0; i<TIMERCOUNT; i++)  {  
//if(timerActive[i] == '1') timerCalc(i); // only the timers that are active 
//
//}
//yield(); // the serialprints last too long with baud 4800 so we reset the wdt
//}  // einde switchcalc

//// *****************************************************************
////                    berekeningen timers van 0 t/m ...
////******************************************************************
void timerCalc(int nummer) { // begint bij 0
//if we calculate this timer, mustCalc[nummer] becomes false  
mustCalc[nummer] = false;
// we kijken eerst of al dit rekenwerk nodig is.
// mutswitch kan alleen gezet worden als weekDag = y  en er waarden in ws_on en sw_off staan en timer[0]=1 mar anders waren we niet hier
// timer[0] = altijd 1 anders waren we niet hier
// er staat altijd een waarde in ws_on en ws_off, hetzij 00:00 of iets anders
// dit hoeven we dus niet te evalueren

// als weekday() + 0*7 -1 (dat is sun=1 dus di =3 -1 = 2
//Serial.println("\n\nshould timer " + String(nummer) + " switch today?" );

//Serial.println("weekday + nummer*7 -1 = " + String(weekday() -1 + nummer*7) );

if (weekDag[weekday()+nummer*7-1] == 'y') {
mustSwitch[nummer] = true;
//Serial.println("mustSwitch[" + String(nummer) + "] = " + String(mustSwitch[nummer]) );
} else {
mustSwitch[nummer] = false; 
return; //stop the calculation
}

// we berekenen dit uit weekDag 0 tm 6, relToSun 0 en 1 en switchOnOff 0 t/m 4 en 5 t/m 9
//time_t schakeltijd = 0;
int swOnuur = 0;
int sw_on = sw_calc(switchOn, nummer*5); //haal de inschakeltijd uit switchOnOff voor timer 0 = 0*5
inschakeltijd[nummer] = schakel_tijd(relToSunOn, nummer, sw_on); //zonattaan1 komt in een char zonat[1] in de functie
int sw_off = sw_calc(switchOff, nummer*5);

uitschakeltijd[nummer] = schakel_tijd(relToSunOff, nummer, sw_off);

// als de tijden aan elkaar gelijk zijn mogen we niet schakelen
// checken of er een y in 
if(uitschakeltijd[nummer] == inschakeltijd[nummer]){ // als ze gelijk zijn dan kijken of er een y is
  // we beginnen bij nummer = 0
     for(int x = 0; x < 7; x++){// de eerste keer 0 1 2 3 4 5 6 en de 2e keer 7 8 etc
          if (weekDag[x] == 'y') { // staat er een y in deze reeks dan is dat fout
                 mustSwitch[nummer] = false;
                 return; // niet verder rekenen  
           }
    }
}
if(uitschakeltijd[nummer] < inschakeltijd[nummer]){
uitschakeltijd[nummer] += 86400; // we tellen er 24 uur bij 
//Serial.println("De uitschakel tijd plus 24 uur in unixtijd uitgedrukt is" + String(uitschakeltijd[nummer]) );
}
DebugPrint("calculated timer "); DebugPrintln(String(nummer));
DebugPrintln ("*****************************************************************");
// deze berekeningen kloppen
yield();
}



// *********************************************************************************
// deze functie berekent de schakeltijden relatief tov zon-op of onder
// *********************************************************************************
time_t schakel_tijd(char relToSun[5], int z, int sw) {
  // we voeden deze functie met de positie in relToSun 0 ,1 ,2 ,3 ,4 etc en sw_of/on 
  char zonat[1];
  time_t schakeltijd = 0;
  
  // z= nummer  dus 0 , 1 , 2, etc
  
  zonat[0] = relToSun[z];

  //DebugPrintln("we zijn in de functie schakel_tijd");
  //DebugPrint("relToSun[z] = ");
  //DebugPrintln(String(relToSun[z]));
  
  //DebugPrint("zonat = ");
  //DebugPrintln(String(zonat[0]));
  //DebugPrint("sw = ");
  //DebugPrintln(sw);

//zonat[0] = relToSun[z];
switch (zonat[0]) { // dit is onbegrijpelijk maar lijkt te werken leest alleen de 1e positie?
  case '0': //absoluut
    //DebugPrint("case was 0 ");
   schakeltijd = sw + dagbegintijd;
//    zonstring_uit = " hr";
    break;
  case '1': //voor zons ondergang
     DebugPrint("case was 1 ");
    schakeltijd = sunset - sw;
//    zonstring_uit = " voor zonsondergang";
    break;
  case '2': //na zons ondergang
    schakeltijd = sunset + sw;
       //DebugPrint("case was 2 ");
 //   zonstring_uit = " na zonsondergang";
    break;
  case '3': // voor zons opkomst
    schakeltijd = sunrise - sw;
       //DebugPrint("case was 3 ");
//    zonstring_uit = " voor zonsopkomst";
    break;
  case '4': // na zons opkomst
    schakeltijd = sunrise + sw;
       //DebugPrint("case was 4 ");
//    zonstring_uit = " na zonsopkomst";
    break;
}
return schakeltijd;
}
// *****************************************************************************
// haalt de schakeltijden op uit switchOn en SwitchOff
int sw_calc(char swits[21], int x) { // x is het mummer van de timer *5 dus 0 ,5 ,10 ets
// als swits = switchOn dan
//als x = 0 dan wordt dit 0 tm 1 en 3 tm 4 2 == de :
//als x = 5               5 tm 7 en 8 tm 10
   int swuur = 0;
          for (int i = x; i < x+2; i++) {
          char c = swits[i];
               if (c < '0' || c > '9') break;
               swuur *= 10;
               swuur += (c - '0');
   //hoe werkt dit? Stel swits=2315
   // in de lus wordt swOnuur de eerste cyclus '2' - '0'
   // de 2e cyclus 2*10 en dan + '3' - '0' = 23
          }

   int swminuut = 0;
          for (int i = x+3; i < x+5; i++) {
          char c = swits[i];
               if (c < '0' || c > '9') break;
               swminuut *= 10;
               swminuut += (c - '0');
          }
            int sw = (swuur * 60 + swminuut) * 60;
            return sw;
}


void tijd_convert () {
cdwnuur = tijd_cvrt( switchcdwn, true );
cdwnminuut = tijd_cvrt( switchcdwn, false );
asouur = tijd_cvrt( aso, true );
asominuut = tijd_cvrt( aso, false );
#ifdef SENSORS
  switch_temp = atof(switchTemp); // switchTemp is een float.
#endif
}
//void tijd_convert() {
////rekent waardes uiut voor asominuut en asouur en cwdnuur /min  
//  //  char b = timer[0];
//
//  char c = switchcdwn[0];
// if (c == '\0') {
//  // als er een letter in staat of niks
// String s = "00:01";
// s.toCharArray(switchcdwn, 6);  
// } 
//     cdwnuur = 0;
//          for (int i = 0; i < 2; i++) {
//          char c = switchcdwn[i];
//               if (c < '0' || c > '9') break;
//               cdwnuur *= 10;
//               cdwnuur += (c - '0');
//          }
//      cdwnminuut = 0;
//          for (int i = 3; i < 5; i++) { // 2 = de :
//          char c = switchcdwn[i];
//               if (c < '0' || c > '9') break;
//               cdwnminuut *= 10;
//               cdwnminuut += (c - '0');
//          }
//
//  c = aso[0];
// if (c == '\0') {
//  // als er een letter in staat of niks
// String s = "06:00";
// s.toCharArray(aso, 6);  
// } 
//     asouur = 0;
//          for (int i = 0; i < 2; i++) {
//          char c = aso[i];
//               if (c < '0' || c > '9') break;
//               asouur *= 10;
//               asouur += (c - '0');
//          }
//      asominuut = 0;
//          for (int i = 3; i < 5; i++) { // 2 = de :
//          char c = aso[i];
//               if (c < '0' || c > '9') break;
//               asominuut *= 10;
//               asominuut += (c - '0');
//          } 
//
//#ifdef SENSORS
//// converteren van de schakeltemperaturen
//  switch_temp = atof(switchTemp); // switchTemp is een float.
//#endif
//}

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

// ****************************************************************
//                     calculate sunset asnd sunrise
// ****************************************************************
void sun_setrise() { // called by gettime

float OUR_longtitude = longi;
float OUR_latitude = lati;
float OUR_timezone = atof(timezone); //   120 // localtime with UTC difference in minutes

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
  mDay = sm.moonDay();
 if (mDay < 13){ maan = "cresc. moon";} //crescent moon"
 if (mDay > 15){maan = "waning moon";}
 if (mDay == 13 || mDay == 14 || mDay == 15){maan = "full moon";}
 if (mDay == 0 || mDay == 1 || mDay == 28){maan = "new moon";} 

      sunrise = sm.sunRise();
      sunset  = sm.sunSet();
      
      if ( zomerTijd ) { // er is zomertijd van toepassing
            if (zomertijd() == true) { // we kijken of de zomertijd van toepassing is
              sunrise = sunrise + 3600; // seconden
              sunset  = sunset + 3600;
              dst = "zomertijd. ";
              } else {
              dst = "wintertijd. ";  
              }
       } else { 
           dst = "geen zomer/wintertijd ingesteld"; 
           }
 }
