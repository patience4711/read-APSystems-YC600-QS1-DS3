#define VERSION  "ESP-ECU_v9_5"


#include <TimeLib.h>
#include <time.h>
#include <sunMoon.h>

#include <LittleFS.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
//#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <Arduino.h>


  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>

#include "ESPAsyncWebServer.h"
AsyncWebServer server(80); //
AsyncWebSocket ws("/ws");

#include <PubSubClient.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
const byte DNS_PORT = 53;
DNSServer dnsServer;
#include "HTML.H"
//#include "AAA_CONFIG_INVERTERS.H"
#include "AAA_MENUPAGE.H"
#include "PORTAL_HTML.H"
//#include "CONFIG_MQTT.H"
//extern "C" {
//    #include "user_interface.h"
//}
/*
 * if we define TEST we have a special environment to test the decoding of a string
 * for the different invertertypes. We only test inv0. If we set this up as a DS3
 * a ds3 string will be analyzed.
 * - no automatic polling
 * - no automatic healthcheck
 * - no start of zigbee at boot\
 * - in the console an option to test inv 0 '10;TESTINV'
 * - more debug information in the console
 */

//#define TEST

#ifdef TEST
int testCounter = 0;
#endif

//#define DEBUG
#ifdef  DEBUG
#define DebugPrint(x)     Serial.print (x)
#define DebugPrintDec(x)  Serial.print (x, DEC)
#define DebugPrintln(x)   Serial.println (x)
#else
#define DebugPrint(x)
#define DebugPrintDec(x)
#define DebugPrintln(x)
#endif

float calliBration = 1.66;
  //bool stoPPed = false;
  bool USB_serial = true;
  char ssid[33] = ""; // was 33 
  char pass[64] = ""; // was 40
  bool tryConnectFlag = false;
// variabelen die met de klok en timers te maken hebben
  WiFiUDP Udp; 
  //time_t daybeginTime = 0;
  time_t switchoffTime = 0;
  time_t switchonTime = 0;
  bool dayTime=true;

  byte mDay = 0;
  String  maan = "";
  int actionFlag = 0;
  //int busyState = 0; // 
  //int pollRes = 300;
 // variabelen wificonfig
  char pswd[11] = "0000";
  char userPwd[11] = "1111";  
  //char lengte[8] = "5.123";
  //char breedte[8] = "51.123";
  float longi = 5.123;
  float lati = 51.123;
  char timezone[5] = "+120";  //+5.30 GMT
  bool zomerTijd = true;
  char static_ip[16] = "000.000.000.000";


// ***************************************************************************
//                             variables ECU
// ***************************************************************************
// ZIGBEE_globals
#define CC2530_MAX_SERIAL_BUFFER_SIZE 512
#define YC600_MAX_NUMBER_OF_INVERTERS 9
char inMessage[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
int readCounter = 0;
char messageHead[5];
bool diagNose=false;
bool Polling = true; // when true we have automatic polling
int errorCode=10;
int recovered = 0;

  int t_saved[YC600_MAX_NUMBER_OF_INVERTERS] = {0};
  float en_saved[YC600_MAX_NUMBER_OF_INVERTERS][4] = {0};
  
  char txBuffer[50];

typedef struct{
  char invLocation[13] = "N/A";
  char invSerial[13]   = "000000000000";
  char invID[7]        = "0x0000";
  int  invType         = 0;
  int  invIdx          = 0;
  bool conPanels[4]    = {true,true,true,true};
} inverters; 
inverters Inv_Prop[9]; 

typedef struct{ 
char freq[5] = "00.0";
char sigQ[5] = "00.0";
char heath[6] = "000.0";
char acv[6] = "000.0";
char dcc[4][5] = {"00.0", "00.0", "00.0", "00.0"};              // ampere <100 
char dcv[4][5] = {"00.0", "00.0", "00.0", "00.0"};              // volt <100
char power[5][6]={"000.0", "000.0", "000.0", "000.0", "000.0"}; //watt < 1000
float en_total = 0;
} inverterdata;
inverterdata Inv_Data[9];
  
 
 bool polled[9]={false,false,false,false,false,false,false,false,false};
 uint8_t zigbeeUp = 11; // initial allways initializing 0 = up
 int pollOffset = 0;
 int inverterKeuze=0;
 int inverterCount=0;
 char ECU_ID[13] = "D8A3011B9780";


 
 String requestUrl = ""; // to remember where we came from  
 //variabelen basis config
// int     hc_IDX = 123;  


// variables mqtt ********************************************
String  Mqtt_Broker=    "192.168.0.100";
String  Mqtt_inTopic =  "ECU/in";
String  Mqtt_outTopic = "domoticz/in";
String  Mqtt_Username;
String  Mqtt_Password;
String  Mqtt_Port =     "1883";
bool    Mqtt_Enabled =  false;
 
  int event = 0;

  int dst;
  int iKeuze;
  bool timeRetrieved = false;
  int networksFound = 0;
  int datum = 0; //


  unsigned long previousMillis = 0;        // will store last temp was read
//  int led_state = 0;

  static unsigned long laatsteMeting = 0; //wordt ook bij OTA gebruikt en bij wifiportal
  static unsigned long lastCheck = 0; //wordt ook bij OTA gebruikt en bij wifiportal

#define LED_AAN    LOW   //sinc
#define LED_UIT    HIGH
#define knop              0  //
#define led_onb           2  // onboard led was 2
#define ZB_RESET          14 // D5
#define ZB_TX             15 // D8
String toSend = "";
 
int value = 0; 

int aantal = 0;
int resetCounter=0;

// *******************************  log *************************************
// variables To record and display last events on the home page.
struct logEvent {
  String    Log_date ;
  String    Log_kind ;
  String    Log_message;
};
//bool Log = false;
static const int Log_MaxEvents = 20;    // Please avoid to make a too big value here, the past events will be stored in memory and displayed in the html home
bool Log_MaxReached = false;
byte Log_CurrentEvent = 0;
logEvent Log_EventList[Log_MaxEvents];  // To stock in memory the last x events (20 by default)


WiFiClient My_WifiClient ;
PubSubClient MQTT_Client ( My_WifiClient ) ;


// *****************************************************************************
// *                              SETUP
// *****************************************************************************

void setup() {


  Serial.begin(115200);
  
  pinMode(knop, INPUT_PULLUP); // de knop
  pinMode(led_onb, OUTPUT); // onboard led
  pinMode(ZB_TX, OUTPUT);
  digitalWrite(ZB_TX, LOW); //
  pinMode(ZB_RESET, OUTPUT);// resetpin cc2530   
  digitalWrite(ZB_RESET, HIGH);
  pinMode(4, OUTPUT); // 
  digitalWrite(4, LOW); //
  
  ledblink(1, 800);

   attachInterrupt(digitalPinToInterrupt(knop), isr, FALLING);

  SPIFFS_read();
 
//  wifi_set_phy_mode(PHY_MODE_11B);
  
  // takes care for the return to the last page after reboot
  read_eeprom(); // put the value of requestUrl back
  if (requestUrl.indexOf("/") == -1) requestUrl="/"; // vangnet
  DebugPrint("requestUrl = "); DebugPrintln(requestUrl);
  
  start_wifi(); // start wifi and server
 

  //currentSecond = now();
  getTijd(); // eenmalig de tijd ophalen
 

 // lees de inverter bestanden
 //Serial.println("setup inverterCount = " +  String(inverterCount));
  for (int x=0; x < inverterCount+1; x++) 
  {
  String bestand = "/Inv_Prop" + String(x) + ".str";  
  DebugPrint("reading file "); DebugPrintln(bestand);
      if (!leesStruct(bestand)) 
      { 
        // if the file not exists we break
        break;
      }
  }
  #ifdef DEBUG
  //printInverters(); // show the inverter files
  #endif

  Update_Log("system", "boot up");
//  // ****************** mqtt init *********************
       MQTT_Client.setServer(Mqtt_Broker.c_str(), Mqtt_Port.toInt());
       MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;

  if ( Mqtt_Enabled ) 
  {
       Serial.println("setup: mqtt configure");
       mqttConnect(); // mqtt verbinden
  } 
  else 
  {
       Update_Log("mqtt", "not enabled"); 
  }
  initWebSocket();
    
  DebugPrintln("ATTENTION DEBUG DEFINED");
  Serial.println("booted up, checking coordinator");
  Serial.println(WiFi.localIP());
  #ifndef DEBUG 
  Serial.println("swapping serial to zb, goodbye");
  swap_to_zb(); // permanent zigbee serial 
  #endif
  delay(1000);
  ledblink(3,500);
  #ifndef TEST
  Update_Log("zigbee","initial healthcheck");
  healthCheck(); // check the state of the zigbee system and if oke then poll the inverters
  if(zigbeeUp == 1) {
        Update_Log("zigbee","coordinator up");
          // we poll our inverters immediatly
          if(Polling)
          {
          for(int i=0; i<inverterCount; i++)
          {     
              if(String(Inv_Prop[i].invID) != "0x0000") polling(i);
          }
          }
     }
  #endif
  resetCounter = 0;
//  stoPPed = false;

#ifdef TEST
t_saved[0] = 36587;
#endif 

} // end setup()
//****************************************************************************
//*                         LOOP
//*****************************************************************************
void loop() {



// ***************************************************************************
//                       day or night mode
// ***************************************************************************

   if(now() > switchonTime && now() < switchoffTime) 
    {
          if(!dayTime)  
          {
             dayTime = true;
             String term= "wake-up from nightmode";
             Update_Log("system", term);
             ws.textAll(term);
             // reset the dayly energy at wakeup and send mqtt message
             resetValues(true, true);
            }
    } else {
         if(dayTime) 
         {
            dayTime = false;
            String term= "start nightmode";
            Update_Log("system", term);
            ws.textAll(term);
            // clean memory
            memset( &inMessage, 0, sizeof(inMessage) ); //zero out the 
            delayMicroseconds(250);
            // we send null messages for each inverter
            resetValues(false, true); // make all values zero exept energy and send mqtt
            actionFlag = 301; // triggers the reset values and mqtt null message at midnight
         }
    }

#ifndef TEST
// ******************************************************************
//              polling every 300 seconds
// ******************************************************************

  unsigned long nu = millis();  // de tijd dat het programma al loopt
   if (nu - laatsteMeting >= 1000UL * 300) 
   {
        laatsteMeting += 1000UL * 300 ; // wordt iedere keer (meetRes * miliseconden) groter;
         //if(now() > switchonTime && now() < switchoffTime )
         if(dayTime && Polling) // we only poll at day and when Polling = true 
         { //only at day
            ledblink(1,100);
            for(int i=0; i<inverterCount; i++)
            {     
              if(String(Inv_Prop[i].invID) != "0x0000") polling(i);
            } 
        } 
 }



// ******************************************************************
//              healthcheck every 10 minutes
// ******************************************************************

   nu = millis();
   if (nu - lastCheck >= 1000UL * 600) // was 600
   {
         lastCheck += 1000UL * 600;
         //we dont do healtcheck when stopped
         healthCheck(); // checks zb radio, mqtt and time, when false only message if error
   }
#endif

  // we recalcultate the switchtimes for this day when there is a new date
  // if retrieve fails, day will not be datum, so we keep trying
  if (day() != datum && hour() > 2) // if date overflew and later then 2
  { 
          getTijd(); // retrieve time and recalculate the switch times
          //DebugPrintln("date overflew, retrieve time");
  }
 
//  if(Mgtt_Enabled) { if (!MQTT_Client.loop()) mqttConnect; }
  
  if(Mqtt_Enabled) {
   if(!MQTT_Client.loop()) mqttConnect();
}
   
  //*********************************************************************
  //             send null data at midnight 
  // ********************************************************************
  if(hour() == 0 && timeRetrieved && actionFlag == 301)
  {
      if(second() > 0 ) 
      {
        resetValues(true, true); //set all values to zero and sent mqtt
        Update_Log("system", "all values reset");
        actionFlag = 0; // to prevent repetition
       }
  }
  
  test_actionFlag();
  
  if( Serial.available() ) {
    empty_serial(); // ignore unexpected incoming
       //handle_Serial();
   }

   ws.cleanupClients();
   yield(); // to avoid wdt resets
}
//****************Einde Loop   *****************************


void ledblink(int i, int wacht) {
  for(int x=0; x<i; x++) {
    digitalWrite(led_onb, LED_AAN);
    delay(wacht);
    digitalWrite(led_onb, LED_UIT);
    delay(wacht);
   }
}
void swap_to_usb () {
  delay(1000);
  if (!USB_serial)
  {
      Serial.swap();
      delay(300);
      empty_serial(); // remove what's in the buffer
      USB_serial=true;
  } 
}
void swap_to_zb () {
   delay(200); //was 300
  if (USB_serial) 
  {
      Serial.swap();
      delay(100);
      empty_serial(); // remove what's in the buffer
      USB_serial=false;
   }
}

void empty_serial() { // remove any remaining data in serial buffer
  while(Serial.available()) {
    Serial.read();
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
    String ntpPool = "";
  } data;

 strcpy( data.str, requestUrl.c_str() ); 
 data.haha = iKeuze;
    EEPROM.put(0, data);
    EEPROM.commit();
}
void read_eeprom() {
    EEPROM.begin(24);

  struct { 
    char str[16] = "";
    int haha = 0;
    String ntpPool = "";
  } data;

EEPROM.get(0, data);
//Serial.println("read value from  is " + String(data.str));
requestUrl=String(data.str);
iKeuze = data.haha; // inverterkeuze
EEPROM.commit();
}


    void test_actionFlag() {
  // ******************  reset the nework data and reboot in AP *************************
    if (actionFlag == 11 || value == 11) { // 
     //DebugPrintln("erasing the wifi credentials, value = " + String(value) + "  actionFlag = " + String(actionFlag));
     delay(1000); //reserve time to release the button
     eraseWifiFlash();
     WiFi.disconnect(true);
     //Serial.println(F("wifi disconnected"));
     ESP.restart();
  }  

    if (actionFlag == 10) { // de knop is langdurig ingedrukt geweest, opstarten in AP
     delay(2000); // give the server the time to send the confirm
     DebugPrintln("rebooting");
     write_eeprom();
     ESP.restart();
  }
    if (actionFlag == 60) {
      actionFlag = 0; //reset the actionflag
      pairOnActionflag();
    }
    if (actionFlag == 21) {
      actionFlag = 0; //reset the actionflag
      coordinator(true); // start normal operation mode
    }
//    if (actionFlag == 22) {
//      actionFlag = 0; //reset the actionflag
//      coordinator(false); // start pairing mode
//    }
#ifdef TEST
    if (actionFlag == 122) {
      actionFlag = 0; //reset the actionflag
      testDecode(); // test a fake pollinganswer for the invType
    }
//    if (actionFlag == 123) {
//      actionFlag = 0; //reset the actionflag
//      testDecode(); // test a fake pollinganswer for qs
//    }    
#endif
    
    if (actionFlag == 24) {
        actionFlag = 0; //reset the actionflag
       //reset the mqttserver
        MQTT_Client.setServer(Mqtt_Broker.c_str(), Mqtt_Port.toInt());
        MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;
        if (Mqtt_Enabled) mqttConnect(); // reconnect mqtt after change of settings
    }    
    if (actionFlag == 25) {
      actionFlag = 0; //reset the actionflag
      getTijd(); // recalculate time after change of settings
    }
    if (actionFlag == 44) { //triggered by the webpage zbtest
        actionFlag = 0; //reset the actionflag
        healthCheck(); 
    }
    if (actionFlag == 45) { //triggered by the webpage zbtest
        actionFlag = 0; //reset the actionflag
        testMessage();
    }
    // polling a single inverter
    if (actionFlag == 47) { //triggered by the webpage zbtest and mqtt
        actionFlag = 0; //reset the actionflag
        polling(iKeuze); 
    }
    //polling all inverters
    if (actionFlag == 48) { //triggered by the webpage zbtest and mqtt
        actionFlag = 0; //reset the actionflag
            ledblink(1,100);
            for(int i=0; i<inverterCount; i++)
            {     
              if(String(Inv_Prop[i].invID) != "0x0000") polling(i);
            } 
    }

}

 void eraseWifiFlash() {
         noInterrupts();
         const size_t cfgSize = 0x4000;
         size_t cfgAddr = ESP.getFlashChipSize() - cfgSize;

        for (size_t offset = 0; offset < cfgSize; offset += SPI_FLASH_SEC_SIZE) {
            if (!ESP.flashEraseSector((cfgAddr + offset) / SPI_FLASH_SEC_SIZE)) {
                Serial.println("erase flash fail");
            }
        }
        interrupts();
 }
