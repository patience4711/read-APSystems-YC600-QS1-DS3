//void handle_Serial () {
//      DebugPrintln(F("we are in handle serial"));  
//      int SerialInByteCounter = 0;
//      char InputBuffer_Serial[100] = "";
//      byte SerialInByte;  
//      delay(200); // wait untill maybe more data available
//      DebugPrintln("normal serial: data available, number = " + String(Serial.available()) );
//  
//      while(Serial.available()) {
//             SerialInByte=Serial.read(); 
//             //Serial.print("+");
//            
//            if(isprint(SerialInByte)) {
//              if(SerialInByteCounter<100) InputBuffer_Serial[SerialInByteCounter++]=SerialInByte;
//            }    
//            if(SerialInByte=='\n') {                                              // new line character
//              InputBuffer_Serial[SerialInByteCounter]=0;
//              DebugPrintln(F("found new line"));
//             break; // serieel data is complete
//            }
//       }  
//     DebugPrintln("InputBuffer_Serial = " + String(InputBuffer_Serial) );
//     // evaluate the incomming data
//     if (strlen(InputBuffer_Serial) > 6) {                                // need to see minimal 8 characters on the serial port
//       if (strncmp (InputBuffer_Serial,"10;",3) == 0) {                 // Command from Master to RFLink
//  
//          if (strncasecmp(InputBuffer_Serial+3,"LIST",4) == 0) {
//              Serial.println(F("*** AVAILABLE COMMANDS ***"));
//              Serial.println(F("10;POLL=x; (poll inverter nr x)"));                            
//              Serial.println(F("10;INIT_N; (start coordinator for polling)"));
//              Serial.println(F("10;INIT_P; (start coordinator for pairing)"));
//              Serial.println(F("10;ERASE; (delete all inverter configfiles)"));
//              Serial.println(F("10;DELETE=<file>; (delete a file)"));              
//              Serial.println(F("10;HEALTH; (perform healthcheck zigbee)"));             
//              Serial.println(F("10;INV_REBOOT; (reboot an unresponsive inverter)"));
//              Serial.println(F("10;CC2530_RST; (reset the cc2530 via its resetpin)"));
//              Serial.println(F("10;CC2530_CFG; (general config cc2530)"));
//              return;
//          } else 
//
//          if (strncasecmp(InputBuffer_Serial+3,"INV_REBOOT",10) == 0) {
//              Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
//              Serial.println(F("\nYou can reboot an inverter when it stopped working."));
//              Serial.println(F("Characteristics: not responsive, (slow blinking red led)."));
//              Serial.println(F("\ntype REBOOT_INVERTER=x (x=inverternumber 0, 1 etc.)"));
//              Serial.println(F("\nDISCLAIMER: THIS HAS NOT BEEN TESTED, USE AT YOUR OWN RISK!"));
//              return;
//          } else
//              
//          if (strncasecmp(InputBuffer_Serial+3,"REBOOT_INVERTER=",16) == 0) {
//
//              int kz = String(InputBuffer_Serial[19]).toInt();
//              Serial.println("\n\nreboot inverter " + String(kz));
//              if ( kz > inverterCount-1 ) 
//              {
//                 Serial.println("error, non-excisting inverter");
//                 return;  
//              }
//                 inverterReset(kz);
//              return;
//          } else 
//
//          if (strncasecmp(InputBuffer_Serial+3,"POLL=",5) == 0) {
//            //input can be 10;POLL=0;  
//              int kz = String(InputBuffer_Serial[8]).toInt();
//              if ( kz > inverterCount-1 ) {
//              Serial.println("error, non-excisting inverter");
//              return;  
//              }
//              Serial.println("poll inverter " + String(kz));
//              polling(kz);
//              return;
//          } else
//           
//          if (strncasecmp(InputBuffer_Serial+3,"FAKE=",5) == 0) {
//            //input can be 10;POLL=0;  
//              int kz = String(InputBuffer_Serial[8]).toInt();
//              Serial.println("simulate inverter " + String(kz));
//              fakeInverter(kz);
//              return;
//          } else
//
//
////           if (strncasecmp(InputBuffer_Serial+3,"PAIR",7) == 0) {  
////              Serial.println("going to setup zigbee coordinator 0");
////              pairing(0);
////              return;             
////          } else  
//
////           if (strncasecmp(InputBuffer_Serial+3,"UNPAIR=",6) == 0) {
////            // input can be 10;UNPAIR=0;
////              Serial.println("going to unpair the specified inverter");
////              int kz = String(InputBuffer_Serial[10]).toInt(); 
////              Serial.println("going to unpair inverter " + String(kz));
////              strcpy(Inv_Prop[kz].invID, "0000"); //unpair
////                writeStruct("/Inv_Prop" + String(kz) + ".str", kz); 
////              return;             
////          
////          } else 
//
////           if (strncasecmp(InputBuffer_Serial+3,"TEST",4) == 0) {  
////              Serial.println("going to read the zigbee radio");
////              readZigbeeTest();
////              return;             
////          } else
//
////            if (strncasecmp(InputBuffer_Serial+3,"REORG",4) == 0) {  
////            Serial.println("going to remove gaps in inverter configfiles ");  
////               remove_gaps();
////               return;             
//////          
////           } else                        
//
//           if (strncasecmp(InputBuffer_Serial+3,"DELETE=",7) == 0) {  
//              //input can be 10;DELETE=filename
//              String bestand="";
//              int z = sizeof(InputBuffer_Serial);
//              for(int i=10;  i<z; i++) {
//                bestand += String(InputBuffer_Serial[i]);
//              }
//              //Serial.println("input filename = " + bestand);
//              Serial.println("going to delete file " + bestand);
//              if (LittleFS.exists(bestand)) { 
//                LittleFS.remove(bestand);
//               Serial.println("file "+ bestand + " removed!"); 
//                } else {Serial.println("no such file");}
//              return;             
////          
//          } else
//          
//           if (strncasecmp(InputBuffer_Serial+3,"ERASE",5) == 0) {  
//              Serial.println("going to delete all inverter files");
//              String bestand;
//              for(int i=0; i<50; i++) 
//              {
//                  String bestand = "/Inv_Prop" + String(i) + ".str";
//                  if (LittleFS.exists(bestand)) 
//                  {
//                      LittleFS.remove(bestand);
//                      Serial.println("removed file " + bestand);
//                  }
//
//              }
//              inverterCount = 0;
//              basisConfigsave(); // save inverterCount
//              Serial.println("done");
//              return;             
//          
//          } else          
//
//           if (strncasecmp(InputBuffer_Serial+3,"INIT_N",6) == 0) {  
//              Serial.println("init the zb system for normal operation");
//              coordinator(true); // start coordinator for normal ops
//              //sendNO(); // send an extra command to establish normal operation
//              //checkZigbeeRadio(true);
//              return;             
//          } else
//           
//           if (strncasecmp(InputBuffer_Serial+3,"INIT_P",6) == 0) {  
//              Serial.println("init the zb system for pairing");
//              coordinator(false); // start coordinator for pairing
//              //checkZigbeeRadio();
//              return;             
//          } else
//
//           if (strncasecmp(InputBuffer_Serial+3,"HEALTH",6) == 0) {  
//              Serial.println("checking the zb radio");
//              healthCheck(true);
//              return;             
//          } else
//
//           
////           if (strncasecmp(InputBuffer_Serial+3,"CC2530_CFG",10) == 0) {  
////              Serial.println("send general config cmds to zb radio ");
////              cc2530Config();
////              return;             
////          } else
//           
//           if (strncasecmp(InputBuffer_Serial+3,"CC2530_RST",10) == 0) {  
//  
//              Serial.println("hard reset the cc2530");
//              ZBhardReset();
//              return; 
////          } else
//
////           if (strncasecmp(InputBuffer_Serial+3,"PAIRSIM",7) == 0) {  
////              strncpy(Inv_Prop[1].invID, "ABCD",6);
////              Serial.print("Inv_Prop[1].invID = " + String(Inv_Prop[1].invID));
////              String bestand = "/Inv_Prop1.str"; // /Inv_Prop1.str
////              writeStruct("/Inv_Prop1.str", 1); // alles opslaan in SPIFFS
////              return;
////          } else
//
////
////           if (strncasecmp(InputBuffer_Serial+3,"TESTQS1",6) == 0) {  
////              Serial.println("going to analyze a QS1 string");
////              qstest(1);
////              return; 
//   
//          } else {
//          
//          Serial.println( String(InputBuffer_Serial) + " INVALID COMMAND" );     
//          }
//       } // end if if (strncmp (InputBuffer_Serial,"10;",3) == 0)
//    Serial.println( String(InputBuffer_Serial) + " UNKNOWN COMMAND" );
//    } //  end if strlen(InputBuffer_Serial) > 6
//  // the buffercontent is not making sense so we empty the buffer
//  empty_serial();
//  } // 
