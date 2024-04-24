// for ota
//#include "esp_http_client.h"
//#include "esp_https_ota.h"
//#include "esp_crt_bundle.h"
//#include <AsyncTCP.h> 
//#include <ESPAsyncWebServer.h>
//AsyncWebServer asyserver(80);//

#include <WiFi.h>
#include <esp_http_server.h>
#include "esp_event.h"
#define QUERYKEY_MAX_LEN  (64)

#include <mbedtls/base64.h>
#include <DNSServer.h> 
//#include "BluetoothSerial.h"
#include "OTA.H"
#include <Update.h>
#include <Hash.h>

#define VERSION  "ESP32C3-SWITCH-v0_0"

//#include <TimeLib.h>
#include <time.h>
#include <sunMoon.h>


//#include "soc/soc.h" // ha for brownout
//#include "soc/rtc_cntl_reg.h" // ha for brownout
//#include <esp_task_wdt.h> // ha
//#include "soc/rtc_wdt.h"
           
#include "SPIFFS.h"
#include "FS.h"
#include <EEPROM.h>
#include <ArduinoJson.h>
//#include "AsyncJson.h"
#include <Arduino.h>

#include <PubSubClient.h>

#include "HTML.H"
#include "AAA_MENUPAGE.H"
#include "PORTAL_HTML.H"
#include "TIMERS_HTML.H"

#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

httpd_handle_t mySocketHD;
int mySocketFD;

  // defines for the pwm
  #define LEDC_TIMER              LEDC_TIMER_0
  #define LEDC_MODE               LEDC_LOW_SPEED_MODE
  #define LEDC_OUTPUT_IO          (5) // Define the output GPIO
  #define LEDC_CHANNEL            LEDC_CHANNEL_0
  #define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
  #define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
  #define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz

  char dvName[21] = ""; // devicename
  char ssid[33] = ""; // was 33 
  char pass[64] = ""; // was 40
  bool tryConnectFlag = false;

  WiFiUDP Udp; 

// variables concerning time and timers
  time_t dagbegintijd = 0;
  time_t uitschakeltijd[4] = {0};
  time_t inschakeltijd[4] = {0};
  time_t inschakeltijdstip = 0;

  int dst;
  char timerActive[5] = "0000";
  char relToSunOn[5] = "0000"; 
  char relToSunOff[5] = "0000"; 
  char switchOn[21] = "00:0000:0000:0000:00";
  char switchOff[21] = "00:0000:0000:0000:00";
  char weekDag[29] = "nnnnnnnnnnnnnnnnnnnnnnnnnnnn";  
  char aso[6] = "06:00";
  long asoTime = 0;
  bool timeRetrieved = false;

 // variables wificonfig
  char pswd[11] = "0000";
  char userPwd[11] = "1111";  
  float longi = 5.123;
  float lati = 51.123;
  char gmtOffset[5] = "+120";  //+5.30 GMT
  bool zomerTijd = true;
  char static_ip[16] = "000.000.000.000";
  uint8_t securityLevel = 6;
  char txBuffer[50];
  #define TIMERCOUNT  4
  bool mustSwitch[4] = {false, false, false, false};
  bool hasSwitched[4] = {false, false, false, false};
  time_t sunset = 0;
  time_t sunrise = 0;  

// variables mqtt ********************************************
  char  Mqtt_Broker[30]=    {"192.168.0.100"};
  char  Mqtt_outTopic[26] = {"domoticz/in"}; // was 26
  char  Mqtt_Username[26] = {""};
  char  Mqtt_Password[26] = {""};
  char  Mqtt_Clientid[26] = {""};
  char  Mqtt_Port[5] =  {"1883"};
  int   Mqtt_switchIDX = 123;
  int   Mqtt_Format = 0; 
  int   event = 0;
  long  mqtt_lastConnect = 0;

  //int dst;

  //char messageHead[5];
  int diagNose = 1; // initial true, can be set in serial (saved in eeprom)
  int tKeuze = 0;
  uint8_t actionFlag = 0;
  char InputBuffer_Serial[50]; // need to be global
  char requestUrl[12] = {""}; // to remember from which webpage we came  

  int networksFound = 0; // used in the portal
  int datum = 0; //

  unsigned long previousMillis = 0;        // will store last temp was read
  static unsigned long laatsteMeting = 0; //wordt ook bij OTA gebruikt en bij wifiportal
  static unsigned long lastCheck = 0; //wordt ook bij OTA gebruikt en bij wifiportal

  #define LED_AAN    LOW   //sinc
  #define LED_UIT    HIGH
  #define knop              0  //
  #define led_onb           8  // onboard led was 2
  //#define ZB_RESET          5 // D5
  //#define ZB_TX             17 // D8

  
  String toSend = ""; // we use this string only to send webpages
  int duty = 256; //pwm dutycycle
  int ledState = 0; // on / off flag 

  WiFiClient espClient ;
  PubSubClient MQTT_Client ( espClient ) ;
  
  //setup pwm
  const int freq = 5000;
  const int ledChannel = 0;
  const int resolution = 8;

// *****************************************************************************
// *                              SETUP
// *****************************************************************************

void setup() {
  Serial.begin(115200);
  
  pinMode(knop, INPUT_PULLUP); // de knop
  pinMode(led_onb, OUTPUT); // onboard led


  pinMode(4, OUTPUT); // 
  digitalWrite(4, LOW); //

  example_ledc_init(); //initialize pwm
  //methods we can use with pwm
  //ledc_set_freq(LEDC_MODE, LEDC_TIMER, 2500); adapt frequency
  //uint32_t ledc_get_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
  //ledc_set_fade_with_time(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int max_fade_time_ms)
  
  //ledc_channel_config(LEDC_CHANNEL)
  // with ledc_set_duty() and ledc_update_duty we change dutycycle
  set_pwm(duty); //to set and update the pwm
  //ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
      // Update duty to apply the new value
  //ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

  ledblink(1, 800);

   attachInterrupt(knop, isr, FALLING);

  SPIFFS_read();
  delay(500); // for the print to complete

  // takes care for the return to the last webpage after reboot
  read_eeprom(); // put the value of requestUrl back an read diagNose
  if (requestUrl[0] != '/' ) strcpy(requestUrl, "/");  // vangnet
  
  consoleOut("requestUrl = " + String(requestUrl));
  start_wifi(); // start wifi and server
  Serial.println("requestUrl at boot = " + String(requestUrl) );

  getTijd(); // retrieve time from the timeserver

  // ****************** mqtt init *********************
  MQTT_Client.setKeepAlive(150);
  MQTT_Client.setServer(Mqtt_Broker, atoi(Mqtt_Port));
  MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;

  if ( Mqtt_Format != 0 ) 
  {
       Serial.println(F("setup: mqtt configure"));
       mqttConnect(); // mqtt connect
  } 
    
  Serial.println(F("booted up"));
  Serial.println(WiFi.localIP());

  delay(1000);
  ledblink(3,500);


  eventSend(0); //update the webpage

} // end setup()

//****************************************************************************
//*                         LOOP
//*****************************************************************************
void loop() {
  // erorchcks
  int aantal = 0;
  if (dagbegintijd < 10000 && aantal < 3 && WiFi.status() == WL_CONNECTED) {
    getTijd(); // does all the calculations
    aantal += 1;
  } //

  // we recalculate the switchtimes for this day when there is a new date
  // if retrieve fails, day will not be datum, so we keep trying by healthcheck
  if (day() != datum && hour() > 2) // if date overflew and later then 2
  { 
     getTijd(); // retrieve time and recalculate the switch times
  }
 
// ****************************************************************************
//                switch by timers
// ****************************************************************************  
   schakelen();


// ***************************************************************************
//                       m q t t
// ***************************************************************************
   // before each transmission the connection is tested
   // so we don't do this in the loop
   if(Mqtt_Format != 0 ) MQTT_Client.loop(); //looks for incoming messages
    
  test_actionFlag();
  
// ****************** vergeten lamp beveiliging *********************************************

if(!hasSwitched[0] && !hasSwitched[1] && !hasSwitched[2] && !hasSwitched[3] ) // not switched by a timer 
   {
      if(ledc_get_duty(LEDC_MODE,LEDC_CHANNEL) > 0) // the leds are on 
      {
           if (now() > inschakeltijdstip + asoTime) // asoTime in secs 
           //if (now() > inschakeltijdstip + ((asouur * 60 + asominuut) * 60))
           {
              consoleOut(" switched off by security");
              ledsOffNow(true, false, "aso");
           }
       }
   }  

   yield(); // to avoid wdt resets

  // SERIAL:******* see if there is data on serial port **********
  if(Serial.available()) {
       handle_Serial();
   }

  delay(100); // to improve responsiveness
}
//****************  End Loop   *****************************

// flash the led
void ledblink(int i, int wacht) {
  for(int x=0; x<i; x++) {
    digitalWrite(led_onb, LED_AAN);
    ledcWrite(0,100);   
    delay(wacht);
    digitalWrite(led_onb, LED_UIT);
    ledcWrite(0,256);
    delay(wacht);
   }
}

  // ****************************************************************
  //                  eeprom handlers
  //*****************************************************************
  void write_eeprom() {
      EEPROM.begin(24);
    //struct data
    struct { 
      char str[16] = "";
      int haha = 0;
    } data;
  
   strcpy( data.str, requestUrl ); 
   data.haha = diagNose; // 
      EEPROM.put(0, data);
      EEPROM.commit();
  }
  
  void read_eeprom() {
      EEPROM.begin(24);
      struct { 
      char str[16] = "";
      int haha = 0;
      } data;
    EEPROM.get(0, data);
    Serial.println("read value from EEPROM is " + String(data.str));
    strcpy(requestUrl, data.str);
    diagNose = data.haha; // inverterkeuze
    EEPROM.commit();
  }

  String getChipId(bool sec) {
      uint32_t chipId = 0;
      for(int i=0; i<17; i=i+8) {
      chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    if(sec) return String(chipId); else return "ESP32-C3-" + String(chipId);
  }

  void showDir() {
      char temp[50]={0};
      File root = SPIFFS.open("/");
      File file = root.openNextFile();
      while (file) {
        sprintf(temp, "%s  size %u<br>" , file.name(), file.size() );
        if (diagNose != 0) consoleOut(String(temp));
        delay(100);
        file = root.openNextFile();
      }
    } 

    void eventSend(byte what) {
    delay(800); // because first fading must be completed 
    const static char *sse_format = "data:%s\r\n\r\n";
    char sse_buf[50];
    if (mySocketFD > 0) {
       if (what == 1) {
             //events.send( "general", "message"); //getGeneral triggered
             snprintf(sse_buf, sizeof(sse_buf), sse_format, "general");
             consoleOut("events message = " + String(sse_buf));
             httpd_socket_send(mySocketHD, mySocketFD, sse_buf, strlen(sse_buf), 0);
         } else {
             //events.send( "getData", "message"); //getGeneral triggered
             snprintf(sse_buf, sizeof(sse_buf), sse_format, "getdata"); 
             consoleOut("events message = " + String(sse_buf));
             httpd_socket_send(mySocketHD, mySocketFD, sse_buf, strlen(sse_buf), 0);
          }
      } else { consoleOut("no events socket available"); }
    }
    // function to 
    void consoleOut(String toLog) {
    // decide to log to serial or the console 
      if(diagNose == 1 ) 
      {
        Serial.println(toLog);
      } else {
        delay(100); // otherwise the socket cannot keep up
      }
   }

   void checkTimers() {
      // if switched off manual this should prevent that this is overruled by a timer so
      //  we disarm it and set it to not switched on
      if ( hasSwitched[0] ) mustSwitch[0] = hasSwitched[0] = false;  
      if ( hasSwitched[1] ) mustSwitch[1] = hasSwitched[1] = false;
      if ( hasSwitched[2] ) mustSwitch[2] = hasSwitched[2] = false;
      if ( hasSwitched[3] ) mustSwitch[3] = hasSwitched[3] = false;        
}

   void ledsOnNow(bool zend, bool check, String who) {
      // we invert the dutyvalue
      //ledcWrite(0,256-duty);
      inschakeltijdstip = now();
      //set_pwm(duty);
      fade_pwm(duty);
      //ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 8191);
      // Update duty to apply the new value
      //ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
      //soft_on();
      Serial.println("on: duty cycle set to " + String(ledc_get_duty(LEDC_MODE,LEDC_CHANNEL)));
      ledState = 1;
      if( zend ) { sendMqttswitch(); }// mqtt switch state
      if( check ) {checkTimers();} // disarm timers that are on  
      //Update_Log(who, "switched off");
      eventSend(0);
    }
    void ledsOffNow(bool zend, bool check, String who) {
        //ledcWrite(0,256);
        //soft_off();
        //set_pwm(0);
        fade_pwm(0);
        //ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
        // Update duty to apply the new value
        //ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        consoleOut("off: duty cycle set to " + String(ledc_get_duty(LEDC_MODE,LEDC_CHANNEL)));
        ledState = 0;
        if( zend ) { sendMqttswitch(); }// mqtt switch state
        if( check ) {checkTimers();} // disarm timers that are on  
        //Update_Log(who, "switched off");
        eventSend(0);
    }
 void  soft_on() {
// we have to go from 256 to 256-duty (duty inverted)
// if current duty is 130 we go from 256 to (256-130 = 126)=256-126 = 130 == duty
// if current duty is 190 we go from 256 to (256-190 = 66)=256-66 = 190 == duty
// the more step the loop counts, the lesser the delay should be so the greater
// duty, the less steps
// i experimented and it must be a value from 2 to 24

// (256 + x ) / y = 12x( 10 + x) / y dus 256 + x = 12( 10+x ) 
//  256 + x = 120 + 12x
//  136 + x = 12x
//  136 = 11x dus 136/11 = x=12,36

  int del = speedCalc(duty);
  for(int x=256; x>=256-duty; x--) {
     ledcWrite(0,x);
     delay(del); // was 10
     }
 }
 void  soft_off() {
// we have to go from (256-duty) to 256 (duty inverted)
// if current duty is 130 we go from (256-130 = 126) to 256 = 256-126 = 130 == duty
// if current duty is 190 we go from (256-130) to 256 = 66) =256-66 = 190 == duty
  int del = speedCalc(duty);
  for(int x=256-duty; x<257; x++) {
     ledcWrite(0,x);
     delay(del); // was 10
     }
 }
 int speedCalc( int input) {
  //Serial.println("speedcalc input = " + String(input));
  //if(input > 150) {Serial.println("return 6"); return 6; } else if (input > 50) {Serial.println("return 8"); return 8;} else {Serial.println("return 10"); return 10;}
  //Serial.println("return = " + String( int(26 - ( (input + 12.436)/11.18) )));
  //return( 28 - ( (input + 40)/12) );
  return( 28 - ( (input + 12.36)/11.18) );
 }


 static void example_ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,      //LEDC_LOW_SPEED_MODE
        .duty_resolution  = LEDC_DUTY_RES,  // LEDC_TIMER_13_BIT
        .timer_num        = LEDC_TIMER,     //LEDC_TIMER_0
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
// the defines for reference
// #define LEDC_TIMER             LEDC_TIMER_0
//#define LEDC_MODE               LEDC_LOW_SPEED_MODE
//#define LEDC_OUTPUT_IO          (5) // Define the output GPIO
//#define LEDC_CHANNEL            LEDC_CHANNEL_0
//#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
//#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
//#define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = LEDC_OUTPUT_IO,  // 5
        .speed_mode     = LEDC_MODE,       // LEDC_LOW_SPEED_MODE
        .channel        = LEDC_CHANNEL,    // LEDC_CHANNEL_0
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER,      //LEDC_TIMER_0 
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_fade_func_install(0);
    // this callback when fading ready
}

void set_pwm(int value) {
  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, value);
  // Update duty to apply the new value
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void fade_pwm(int value) {
  // LEDC_MODE, LEDC_CHANNEL, target duty, time = set in ms
  ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, value, 1000); 
  ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
}
