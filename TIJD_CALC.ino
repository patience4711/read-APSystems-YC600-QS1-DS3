// this function calculates sunrise and sunset time
void sun_setrise() {

  float OUR_longtitude = longi;
  float OUR_latitude = lati;
  float OUR_timezone = atof(timezone); //   120                     // localtime with UTC difference in minutes

  sunMoon  sm;

  sm.init(OUR_timezone, OUR_latitude, OUR_longtitude);

  time_t sunrise = sm.sunRise();
  time_t sunset  = sm.sunSet();
  
  if ( zomerTijd ) { // dst is set
        if (zomertijd() == true) { // we check if it is summertime
          sunrise = sunrise + 3600; // seconds
          sunset  = sunset + 3600;
          dst = 1; //summer time
          } else {
          dst = 2; // winter time 
          }
   } 
   else 
   { 
      dst = 0; 
   }
   switchonTime = sunrise + pollOffset*60; // was -900
   switchoffTime = sunset - pollOffset*60; // was +900
 
}


// a universal function to convert hours and minutes from the char to a string
// if wat true than returns hourse else minutes
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
