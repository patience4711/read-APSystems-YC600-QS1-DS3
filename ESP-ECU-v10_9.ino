
//#include <lwip/tcp_impl.h>
#include <AsyncEventSource.h>
#include <AsyncJson.h>
#include <AsyncWebSocket.h>
#include <AsyncWebSynchronization.h>
#include <ESPAsyncWebSrv.h>
#include <SPIFFSEditor.h>
#include <StringArray.h>
#include <WebAuthentication.h>
#include <WebHandlerImpl.h>
#include <WebResponseImpl.h>

/*  edit readZigbee
 *  made processIncomingByte redundant
 *  moved crc and slen to sendZigbee
 *  added a debugmessage to sendzigbee when (diag)
 *  removed the line 336 memset crashed the zigbee
 *  remove intopic and changed that to clientid
 *  introduced a processor to bring the invertercount to the frontpage
 *  and the polling start/end time
 *  no more need for get.Times.
 *  Splitted up get.Power in a get.General that only loads remote, state and night
 *  the power and energy values are provided by get.Power
 *  introduced arduinojson to make the jsons send to the webpages
 *  disabled the menu links, log is made but cannot be visited.
 *  the call to get.Inverter does not eat memory
 *  if i have the frontpage open not eat memory >28000 start with 29760
 *  opening the detailspage lowers the memory so we separated the script to make the page smaller
 *  still memory lowering but over 28.500 after one hour
 *  seen a memory dip without knowing what happened. Perhaps sending a page while polling? 
 *  Can we block the webserver while polling? Or the other way around?
 *  I intoduced memCheck, only poll when enough mem defau;t 20000
 *  removed the memcheck, stripped off the favicon and the help
 *  changed mqttconfig not using string toSend
 *  to minimize the wifi trafic and to preven collision betweeen cpu hungry polling and webserver
 *  sending large pages, we trigger a page reload and get.data by the software
 *  get.Data after polling or reset values
 *  reload when status has changed eg after boot , sleep wake and when a zigbee start attempt
 *  changed the static jsons in the http requests and remomved the use of jsonObject
 *  load the homepage without the scripts, see if this eats memory
 *  combined the page and java files and removed all wjitepace to make them smaller
 *  heap goes down with 96 every visit of the webpage infopage
 */
#define VERSION  "ESP-ECU_v10_8"

#include <TimeLib.h>
#include <time.h>
#include <sunMoon.h>

#include <LittleFS.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <EEPROM.h>
#include <ArduinoJson.h>
//#include "AsyncJson.h"
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

//#include "ESPAsyncWebServer.h"
AsyncWebServer server(80); //
AsyncEventSource events("/events"); 
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
#include "AAA_HOMEPAGE.H"
#include "PORTAL_HTML.H"
#include "DETAILSPAGE.H"
//#include "ABOUT2.H"
//#include "DIV_HTML.H"
/*
 * if we define TEST we have a special environment to test the decoding of a string
 * for the different invertertypes. We only test inv0. If we set this up as a DS3
 * a ds3 string will be analyzed.
 * - no automatic polling
 * - no automatic healthcheck
 * - no start of zigbee at boot\
 * - in the console an option to test inv 0 '10;TESTINV'
 * - more debug information in the console
 * upgraded from 2.7.1
 */
//#define MEMCHECK // when defined we mqtt memory every 1 minute
//#define TEST
#define LOG
#ifdef TEST
int testCounter = 0;
#endif

////#define DEBUG
//#ifdef  DEBUG
//#define DebugPrint(x)     Serial.print (x)
//#define DebugPrintDec(x)  Serial.print (x, DEC)
//#define DebugPrintln(x)   Serial.println (x)
//#else
//#define DebugPrint(x)
//#define DebugPrintDec(x)
//#define DebugPrintln(x)
//#endif

//float calliBration = 1.66;
  //bool stoPPed = false;
  bool USB_serial = true;
  char ssid[33] = ""; // was 33 
  char pass[64] = ""; // was 40
  bool tryConnectFlag = false;
// variables concerning time and timers
  WiFiUDP Udp; 
  //time_t daybeginTime = 0;
  time_t switchoffTime = 0;
  time_t switchonTime = 0;
  bool dayTime=true;
  uint8_t actionFlag = 0;

 // variables wificonfig
  char pswd[11] = "0000";
  char userPwd[11] = "1111";  
  float longi = 5.123;
  float lati = 51.123;
  char gmtOffset[5] = "+120";  //+5.30 GMT
  bool zomerTijd = true;
  char static_ip[16] = "000.000.000.000";
  int securityLevel = 0;


// ***************************************************************************
//                             variables ECU
// ***************************************************************************
// ZIGBEE_globals
#define CC2530_MAX_SERIAL_BUFFER_SIZE 512
#define MAX_NUMBER_OF_INVERTERS 9
//char inMessage[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
int readCounter = 0;

bool diagNose=true; // we startup with all debug info
bool Polling = true; // when true we have automatic polling
int errorCode=10;


  int t_saved[MAX_NUMBER_OF_INVERTERS] = {0};
  float en_saved[MAX_NUMBER_OF_INVERTERS][4] = {0}; 
  //to remember the value of the previous poll. 
  //This value is the energy that is reported by the inverter
  
  char txBuffer[50];

typedef struct{
  char invLocation[13] = "N/A";
  char invSerial[13]   = "000000000000";
  char invID[5]        = "0000";
  int  invType         = 0;
  int  invIdx          = 0;
  bool conPanels[4]    = {true,true,true,true};
} inverters; 
inverters Inv_Prop[MAX_NUMBER_OF_INVERTERS]; 

typedef struct{ 
float freq = 0.0;
float sigQ = 0.0;
float heath = 0.0;
float acv = 0.0;
float dcc[4] = {0.0, 0.0, 0.0, 0.0};              // ampere <100 
float dcv[4] = {0.0, 0.0, 0.0, 0.0};              // volt <100
float power[4] = {0.0, 0.0, 0.0, 0.0};       //watt < 1000
float pw_total = 0.0;
float en_total = 0;
} inverterdata;
inverterdata Inv_Data[9];
  
 
 
 uint8_t zigbeeUp = 11; // initial allways initializing, this changes to 1 = up or 0 not up after initial healthcheck

 char ECU_ID[13] = "D8A3011B9780";

char requestUrl[12] = {""}; // to remember from which webpage we came  

// variables mqtt ********************************************
  char  Mqtt_Broker[30]=    {"192.168.0.100"};
//  char  Mqtt_inTopic[16] =  {"ESP-ECU/in"};
  char  Mqtt_outTopic[26] = {"domoticz/in"}; // was 26
  char  Mqtt_Username[26] = {""};
  char  Mqtt_Password[26] = {""};
  char  Mqtt_Clientid[26] = {""};
  char  Mqtt_Port[5] =  {"1883"};
  int   Mqtt_Format = 0;

  //long    mqtt_lastConnect = 0;

  int event = 0;

  int dst;
  int iKeuze;
  int inverterTopoll = 0;
  bool timeRetrieved = false;
  int networksFound = 0; // used in the portal
  int datum = 0; //

  unsigned long previousMillis = 0;        // will store last temp was read
  static unsigned long laatsteMeting = 0; //wordt ook bij OTA gebruikt en bij wifiportal
  static unsigned long lastCheck = 0; //wordt ook bij OTA gebruikt en bij wifiportal
#ifdef MEMCHECK
  static unsigned long lastmemCheck = 0; //memory check
#endif  
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
//// variables To record and display last events on the home page.
#ifdef LOG
#define Log_MaxEvents 18 
 
 typedef struct {
  char date[14] ;
  int  kind ; // zigbee, system, mqtt, pairing
  char  message[13] ;
} logEvent;
logEvent Log_Events[Log_MaxEvents];
bool Log_MaxReached = false;
byte logNr = 0;
#endif
WiFiClient espClient ;
PubSubClient MQTT_Client(espClient) ;

int Mqtt_stateIDX = 0;
int pollOffset = 0;
int inverterKeuze=0;
int inverterCount=0;
bool polled[9]; //={false,false,false,false,false,false,false,false,false};
//int checkTeller = 0;

//unsigned int memCheck = 20000;
//int lowmemCounter = 0;

bool getallTrigger = false;
bool reloadTrigger = false;

// *****************************************************************************
// *                              SETUP
// *****************************************************************************


void setup() {
  Serial.begin(115200);

//    // init record of stack
//    char stack;
//    stack_start = &stack;

  
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
 // now we know the number of inverters we can find an interval between pollings
 //int pollintervall = 300/inverterCount;  
// takes care for the return to the last webpage after reboot
  read_eeprom(); // put the value of requestUrl back
  if (requestUrl[0] != '/' ) strcpy(requestUrl, "/");  // vangnet
  Serial.println("\n\nrequestUrl = " + String(requestUrl));
  
  start_wifi(); // start wifi and server

// reed inverter files
  for (int x=0; x < inverterCount+1; x++) 
  {
  String bestand = "/Inv_Prop" + String(x) + ".str";  
  //DebugPrint("reading file "); //DebugPrintln(bestand);
      if (!leesStruct(bestand)) 
      { 
        // if the file not exists we break
        break;
      }
  }
  #ifdef DEBUG
  //printInverters(); // show the inverter files
  #endif
  //test_polling(0);
  getTijd(); // retrieve time from the timeserver
  #ifdef LOG 
  Update_Log(1, "boot up"); 
  #endif

//  // ****************** mqtt init *********************
       MQTT_Client.setKeepAlive(150);
       MQTT_Client.setBufferSize(512);
       MQTT_Client.setServer(Mqtt_Broker, atoi(Mqtt_Port) );
       MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;

  if ( Mqtt_Format != 0 ) 
  {
       Serial.println(F("setup: mqtt configure"));
       mqttConnect(); // mqtt connect
  } 
  else 
  {
#ifdef LOG
   Update_Log(3, "not enabled"); 
#endif 
  }
  initWebSocket();
//       Serial.println("logNr=" + String(logNr));   
  //DebugPrintln("ATTENTION DEBUG DEFINED");
  Serial.println(F("booted up, checking coordinator"));
  Serial.println(WiFi.localIP());
  int hp_len = sizeof(ECU_HOMEPAGE)/sizeof(ECU_HOMEPAGE[0]);
  Serial.println("ECU_HOMEPAGE length = " + String(hp_len) );
  hp_len = sizeof(STYLESHEET_HOME)/sizeof(STYLESHEET_HOME[0]);
  Serial.println("stylesheet_home length = " + String(hp_len) );  
  hp_len = sizeof(STYLESHEET_SUBS)/sizeof(STYLESHEET_SUBS[0]);
  Serial.println("stylesheet_subs length = " + String(hp_len) );   
  hp_len = sizeof(MENUPAGE)/sizeof(MENUPAGE[0]);
  Serial.println("MENUPAGE length = " + String(hp_len));  
  hp_len = sizeof(HTML_HEAD)/sizeof(HTML_HEAD[0]);
  Serial.println("HTML_HEAD length = " + String(hp_len));
  hp_len = sizeof(LOGPAGE)/sizeof(LOGPAGE[0]);
  Serial.println("LOGPAGE length = " + String(hp_len));
  hp_len = sizeof(DETAILSPAGE)/sizeof(DETAILSPAGE[0]);
  Serial.println("DETAILSPAGE length = " + String(hp_len));
//  hp_len = sizeof(DETAILS_SCRIPT)/sizeof(DETAILS_SCRIPT[0]);
//  Serial.println("DETAILS_SCRIPT length = " + String(hp_len));
//  hp_len = sizeof(JAVA_SCRIPT)/sizeof(JAVA_SCRIPT[0]);
//  Serial.println("JAVA_SCRIPT length = " + String(hp_len)); 
    
  
  
  #ifndef DEBUG 
  Serial.println(F("swapping serial to zb, goodbye"));
  swap_to_zb(); // permanent zigbee serial 
  #endif
  delay(1000);
  ledblink(3,500);
  #ifndef TEST
  #ifdef LOG 
  Update_Log(2,"healthcheck"); 
  #endif
  healthCheck(); // check the state of the zigbee system and if oke then poll the inverters
  if(zigbeeUp == 1) {
        #ifdef LOG 
        Update_Log(2,"running"); 
        #endif
          // we poll our inverters immediatly
          if(Polling)
          {
            poll_all();
          }
     }
  #endif
  resetCounter = 0;
//  stoPPed = false;

#ifdef TEST
t_saved[0] = 36587;
zigbeeUp = 1;
//force_values(); // now we have values for all inverters
#endif 
//reloadTrigger = true; // we reload the frontpage to reflect the status
//events.send( "reload", "message");
eventSend(0);
} // end setup()

//****************************************************************************
//*                         LOOP
//*****************************************************************************
void loop() {

// ***************************************************************************
//                       day or night mode
// ***************************************************************************
#ifdef TEST
// always daytime to be able to test
 dayTime = true;
#endif
// when no time is retrieved, swOn an swOff times are null
// so this eveluate to false, so always nightmode 
   if(now() > switchonTime && now() < switchoffTime) 
    {
          if(!dayTime)  
          {
             dayTime = true;
             //String term= "wake-up from nightmode";
             #ifdef LOG 
             Update_Log(1, "woke-up"); 
             #endif
             if(diagNose) ws.textAll(F("wake-up from nightmode"));
             // reset the dayly energy at wakeup and send mqtt message
             resetValues(true, true);
             //events.send( "reload", "message"); // refresh the data and state
             eventSend(0);
            }
    } else {
         if(dayTime) 
         {
            dayTime = false;
            //String term= "start nightmode";
            #ifdef LOG 
            Update_Log(1, "nightmode"); 
            #endif
            if(diagNose) ws.textAll(F("start nightmode"));
            // we send null messages for each inverter
            resetValues(false, true); // make all values zero exept energy and send mqtt
            //events.send( "general", "message"); // refresh the data and state
            eventSend(1);
            actionFlag = 250; // triggers the reset values and mqtt null message at midnight
         }
    }

//              polling every 300 seconds 
// ******************************************************************

  unsigned long nu = millis();  // the time the program is running
//   if (nu - laatsteMeting >= 1000UL * (300 / inverterCount)) 
   if (nu - laatsteMeting >= 1000UL * 300) 
   {
     if(diagNose) ws.textAll( "300 secs passed" ); //
     laatsteMeting += 1000UL * 300 ; // increases each time with (300/inverterCount * miliseconds);
       if(dayTime && Polling && zigbeeUp == 1) // we only poll at day and when Polling = true 
          { 
             ledblink(1,100);
             poll_all(); //if inverterCount = 9 than we have inverters 0-8
          } 
 }
// ******************************************************************
//     healthcheck every 10 minutes but 2 minutes later than poll 
// ******************************************************************
//   nu = millis() + 1000UL * 120 ; // 120 sec = 12 minutes later
   if (nu - lastCheck >= 1000UL * 600) // was 600
   {
         //ws.textAll("600 secs passed" + String(millis()) );
         lastCheck += 1000UL * 600;
         healthCheck(); // checks zb radio, mqtt and time, when false only message if error
   }

// ******************************************************************
//     memory check every minute  
// ******************************************************************
#ifdef MEMCHECK
   if (nu - lastmemCheck >= 1000UL * 60) // was 600
   {
         
         lastmemCheck += 1000UL * 60;
         
         memoryCheck(); // checks free heap
   }
#endif
  // we recalculate the switchtimes for this day when there is a new date
  // if retrieve fails, day will not be datum, so we keep trying
  if (day() != datum && hour() > 2) // if date overflew and later then 2
  { 
          //check if the heap is large enough, else reset
          if (ESP.getFreeHeap() < 16000) ESP.restart();
          getTijd(); // retrieve time and recalculate the switch times
          //delay(500);
          //ESP.restart();
  }
 
  // we do this before sending polling info and healthcheck
  // this way it can't block the loop
  if(Mqtt_Format != 0 ) MQTT_Client.loop(); //looks for incoming messages
 
  //*********************************************************************
  //             send null data at midnight 
  // ********************************************************************
  if(hour() == 0 && timeRetrieved && actionFlag == 250)
  {
      if(second() > 0 ) 
      {
        resetValues(true, true); //set all values to zero and sent mqtt
        #ifdef LOG 
        Update_Log( 1, "values reset"); 
        #endif
        actionFlag = 0; // to prevent repetition
        //events.send( "getall", "message");
        eventSend(2);
        // ZBhardReset(); // reset the zigbeemodule
        // healthCheck(); // this increases the resetcounter so we decrease it
        // resetCounter--; // so that this reset is not counted
       }
  }
  
  test_actionFlag();
  
  if( Serial.available() ) {
    empty_serial(); // clear unexpected incoming data
   }

   ws.cleanupClients();
   yield(); // to avoid wdt resets

}
//****************  End Loop   *****************************

// flash the led
void ledblink(int i, int wacht) {
  for(int x=0; x<i; x++) {
    digitalWrite(led_onb, LED_AAN);
    delay(wacht);
    digitalWrite(led_onb, LED_UIT);
    delay(wacht);
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
  } data;

 strcpy( data.str, requestUrl ); 
 data.haha = iKeuze;
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
  //Serial.println("read value from  is " + String(data.str));
  strcpy(requestUrl, data.str);
  iKeuze = data.haha; // inverterkeuze
  EEPROM.commit();
}

// all actions called by the webinterface should run outside the async webserver environment
// otherwise crashes will occure.
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

    if (actionFlag == 10) { // reboot
     delay(2000); // give the server the time to send the confirm
     //DebugPrintln("rebooting");
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
      //events.send( "general", "message"); // update the data
      eventSend(1);
    }
//    if (actionFlag == 22) {
//      actionFlag = 0; //reset the actionflag
//      coordinator(false); // start pairing mode
//    }
#ifdef TEST
    if (actionFlag == 122) {
      actionFlag = 0; //reset the actionflag
      if(testCounter > 1) { 
        testCounter=0; // reset testcounter
        // make all values null
        resetValues(true, false);
      }
      testDecode(); // test a fake polling answer for the invType
    }
//    if (actionFlag == 123) {
//      actionFlag = 0; //reset the actionflag
//      testDecode(); // test a fake pollinganswer for qs
//    }    
#endif
    
    if (actionFlag == 24) {
        actionFlag = 0; //reset the actionflag
        MQTT_Client.disconnect() ;
       //reset the mqttserver
        MQTT_Client.setServer ( Mqtt_Broker, atoi(Mqtt_Port) );
        MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;
        if (Mqtt_Format != 0) mqttConnect(); // reconnect mqtt after change of settings
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
    if (actionFlag == 46) { //triggered by the webpage zbtest
        actionFlag = 0; //reset the actionflag
        showDir(); 
    }
    
    // polling a single inverter
    if (actionFlag == 47) { //triggered by the webpage zbtest and mqtt
        actionFlag = 0; //reset the actionflag
        polling(iKeuze); 
        //getallTrigger = true; // update the webpage
        //events.send( "getall", "message", millis());
        eventSend(2);
    }
    //polling all inverters
    if (actionFlag == 48) { //triggered by the webpage zbtest and mqtt
        actionFlag = 0; //reset the actionflag
            ledblink(1,100);
            poll_all();    
    }

    if (actionFlag == 49) { //triggered by console testmqtt
        actionFlag = 0; //reset the actionflag
        ledblink(1,100);
        // always first drop the existing connection
        MQTT_Client.disconnect();
        ws.textAll("dropped connection");
        delay(100);
        char Mqtt_send[26] = {0};
       
        if(mqttConnect() ) {
        String toMQTT=""; // if we are connected we do this
        
        strcpy( Mqtt_send , Mqtt_outTopic);
        
        if(Mqtt_send[strlen(Mqtt_send -1)] == '/') strcat(Mqtt_send, String(Inv_Prop[0].invIdx).c_str());
        toMQTT = "{\"test\":\"" + String(Mqtt_send) + "\"}";
        
        if(Mqtt_Format == 5) toMQTT = "field1=12.3&field4=44.4&status=MQTTPUBLISH";
        
        if( MQTT_Client.publish (Mqtt_outTopic, toMQTT.c_str() ) ) {
            ws.textAll("sent mqtt message : " + toMQTT);
        } else {
            ws.textAll("sending mqtt message failed : " + toMQTT);    
        }
      } 
     // the not connected message is displayed by mqttConnect()
    }


} // end test actionflag

 void eraseWifiFlash() {
         noInterrupts();
         const size_t cfgSize = 0x4000;
         size_t cfgAddr = ESP.getFlashChipSize() - cfgSize;

        for (size_t offset = 0; offset < cfgSize; offset += SPI_FLASH_SEC_SIZE) {
            if (!ESP.flashEraseSector((cfgAddr + offset) / SPI_FLASH_SEC_SIZE)) {
                Serial.println(F("erase flash fail"));
            }
        }
        interrupts();
 }

 void poll_all() { 
 // poll all inverters, inbetweeen empty serial and feed wdt 
     for(int i=0; i<inverterCount; i++)
     {     
          if(String(Inv_Prop[i].invID) != "0000") {
                polling(i);
                delay(2000);
                empty_serial(); // no need for?
                ESP.wdtFeed();
          }
       }
       //events.send( "getall", "message"); // update the data
       eventSend(2);
 }
 void showDir() {
    ws.textAll("**** Content filesystem ****");
    char temp[50]={0};
    Dir dir = LittleFS.openDir("/");
    while (dir.next() ) {
          File f = dir.openFile("r");
          sprintf(temp, "%s  size %u<br>" , dir.fileName().c_str(), f.size() );
          ws.textAll(String(temp));
          delay(100);
    }
 }

void eventSend(byte what) {
  if (what == 1) {
      events.send( "general", "message"); //getGeneral triggered            
  } else 
     if (what == 2) {
     events.send( "getall", "message"); //getAll triggered
  } else {  
     events.send( "reload", "message"); // both triggered
  }
}            
