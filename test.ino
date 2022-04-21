
void testMessage() {
char sendCmd[100];
int len = strlen(txBuffer);

DebugPrint("len of txBuffer :  "); DebugPrintln(String(txBuffer));

      for(int i=0; i<len; i++) 
      {
        sendCmd[i] = txBuffer[i+7];
      }
        
       //len at the begin
       strcpy(sendCmd, strncat(sLen(sendCmd), sendCmd, sizeof(sLen(sendCmd)) + sizeof(sendCmd))); //build command plus sln at the beginning
       // CRC at the end 
       strcpy(sendCmd, strncat(sendCmd, checkSum(sendCmd), sizeof(sendCmd) + sizeof(checkSum(sendCmd))));
       ws.textAll("sendCmd = FE" + String(sendCmd));
       //now we send this command
       #ifdef DEBUG
       Serial.println("sendCmd = " + String(sendCmd));
       swap_to_zb();
       #endif
       sendZigbee(sendCmd);
       if ( waitSerialAvailable() ) { readZigbee(); } else { readCounter = 0;}
       #ifdef DEBUG
       swap_to_usb();
       Serial.println("received : " + String(inMessage));
       #endif
       ws.textAll("answer " + String(inMessage));
}


#ifdef TEST
void testDecode() {
// we always test inverter 0, depending on the type we test the right string
int type = Inv_Prop[0].invType; //
// we define an inmessage first
// the time in the message = 36887
//we want a timespan of 300 

//en_saved[0][0] = 245;


switch(type) {
 case 0: // yc600
   strncpy(inMessage, "FE0164010064FE034480001401D2FE0345C43A1000A8FE724481000006013A101414007100B57CFA00005E408000158215FBFB51B103D40F4117000074CF00000076706A73D06B0496000000000000000172072D88017862E8201F00030555073F0303030100000100000000000000000000000000000000000000000000000000000000000000FEFE3A100E76",300);
   break;
 case 1: // test qs1 johan
   strncpy(inMessage, "FE0164010064FE034480001401D2FE72448100000601C0051414005E00905D5B00005E801000085070FBFB51B103EB0F419300CAF069D9F068C7C068C1206804B868E0000006A80001BB38134D01CCE90E0A01FD1E052201D967D0641F0003055400000000000000000000000000000000000000000000000000002B2A0000FEFEC0050E55",300);
   break;
 case 2: // ds3
   if(testCounter == 0) {
// from npeters tracefile at 9:53 and 9:57
   strncpy(inMessage, "FE0164010064FE034480001401D2FE0345C4226C00CCFE0345C4226C00CCFE0345C43A1000A8FE724481000006013A101414007100B57CFA00005E703000021300fbfb5cbbbb2000fc0001ffff000000000000000006e506ee00E200EA036e13882D5F01480026ffff052508430049F8C40048C77C00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff3896fefe",300);   
//                                                                                                                                           >                                                   26                      tttt                    100     108       
   } else {
   strncpy(inMessage, "FE0164010064FE034480001401D2FE0345C4226C00CCFE0345C4226C00CCFE0345C43A1000A8FE724481000006013A101414007100B57CFA00005E703000021300fbfb5cbbbb2000fc0001ffff000000000000000006e506ee015900EF010313882E8B01480026ffff052508430050BF39004F849C00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff3896fefe",300);   
//                                                                                                                                           >                                                   26                      tttt                    100     108       
   }
break;
}
//703000021300 FB FB 5C BB BB 20 00 FC 00 01 FF FF 00 00 00 00 00 00 00 00 06 83 06 8A 02 31 02 3A 03 68 13 87   2C 9B  01 
//703000021300 fb fb 5c bb bb 20 00 02 00 e6 ff ff 00 00 00 00 00 00 00 00 06 f5 06 f9 00 2e 00 34 03 60 13 8a | 17 a7 |00 
//0             6  7  8  9 10 11 12 13 14 15                21 22                      30                      | time  |40                            50       53 54 55

//BB 00 2A FF FF 05 6B 08 D8 01 1C 4E 79 01 1D BD 26 00 FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF3830FEFE226C70E
//24 00 1f ff ff 05 42 06 90 00 16 f6 2b 00 18 e4 51 ff ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff3969fefe
//41          45             50             55  

//ws.textAll("going to test for invType: " + String(type) + "\n");
decodePollAnswer(0);

polled[0]=true;
testCounter += 1; // for the next round we have new string
}
#endif

//int langte = readMessageHead() ; 
//// now we know the length we can read the rest of the message
//readCounter=0;
//
//if(langte > 300) {
//  //Serial.print("error, returning");
//  ws.textAll("length > 300, return");
//  return;
//}
////Serial.println("readCounter = " + String(readCounter));
//// we already read 4 bytes, the message = langte and the crc is not relevand so we stil must read langte bytes
////   swap_to_zb();      
//      
//      for(int z=0; z < langte; z++) // 
//        {
//            processIncomingByteTest(Serial.read()); // format and add the byte to 
//            //Serial.println("inMessage=now: " + String(inMessage));
//            readCounter += 1;
//            
//            //if (Serial.available() == 0 || readCounter==langte+2) break; //done
//        }
//
////#ifdef DEBUG
////           swap_to_usb();      
////           Serial.println("readZigMessage done");
////#endif           
//           //terminate this array
//           //strncat(inMessage, '\0', 2);
//            //convert inMessage to upper
//            uint16_t iToUpper = 0; // has to be 16bit because the received message if the YC600 answers is longer then 255
//            while (inMessage[iToUpper])
//            {
//                inMessage[iToUpper] = toupper(inMessage[iToUpper]);
//                iToUpper++;
//            }        
//#ifdef DEBUG
//Serial.println("@ break = " + String(inMessage));        
//#endif
////        ws.textAll("readCounter @ break = " + String(readCounter));        
//        //Serial.println("available @ break = " + String(Serial.available()));
//        ws.textAll("inMessage: " + String(inMessage));
//}
//
//// *******************************************************************************
////          extract message length ( the byte after FE )
//// *******************************************************************************
//int readMessageHead() {
////Serial.println("readZigbeeHead");
//memset( &inMessage, 0, sizeof(inMessage) ); //zero out the 
//delayMicroseconds(250);
//
//readCounter = 0;
//int lengte=0;
//char len[3];
//bool foundFE = false;
//// read utill we find FE and the next (length) bytes
//        while (Serial.available())
//        {
//           //ledblink(1,50); 
//            processIncomingByteTest(Serial.read());
//            if(readCounter > 0 && inMessage[readCounter] == 'e' && inMessage[readCounter-1] == 'f' ) 
//            {
//                readCounter = 2; // we set readcounter to 2 so we read 2 and 3
//                foundFE = true;
//                //ledblink(1,300);
//                 //ws.textAll("length of inMessage = " + String(strlen(inMessage)));
//            }  
//
//             readCounter += 1;
//             
//             if ( foundFE && readCounter == 6 ) // we have 0 1 2 3 = FE  and length and the command 
//                {
//                     strncpy(messageHead, inMessage, 8); // export the header
//                     ws.textAll("@ rc=4 messageHead = " + String(messageHead));
//                     len[0] = inMessage[2];
//                     len[1] = inMessage[3];
//                     len[2] = '\0';
//                     lengte = (int) strtol(len, 0, 16);
//                     break;
//                }
//        }     
//return lengte; //this function seem to return on itself
//}


//// format the incoming byte and add it to inMessage
//void processIncomingByteTest(const byte inByte)
//{
//char oneChar[10] = {0};
//    sprintf(oneChar, "%02x", inByte);
//    strncat(inMessage, oneChar, 2); // append 
//} // end of processIncomingByte



//void fakeInverter(int which) {
//// we populate a fake sq1 inverter with data
//polled[which] = true;
////String freq = "00.0";
////String heath = "00.0";
////String acv = "000.0";
////String dcc[4] = {"00.0", "00.0", "00.0", "00.0"}; // ampere <100 (
////String dcv[4] = {"00.0", "00.0", "00.0", "00.0"}; // volt <100
////String power[5]={"000.0", "000.0", "000.0", "000.0", "000.0"}; //watt < 1000
//
//strncpy(Inv_Data[which].freq, "55.0", 4);
//strncpy(Inv_Data[which].heath, "30.3" ,4);
//strncpy(Inv_Data[which].acv, "222.2", 4);
//strncpy(Inv_Data[which].dcv[0], "31.0", 4); 
//strncpy(Inv_Data[which].dcv[1], "31.1", 4); 
//strncpy(Inv_Data[which].dcv[2], "31.2", 4);
//strncpy(Inv_Data[which].dcv[3], "31.3", 4);
//strncpy(Inv_Data[which].dcc[0], "10.0", 3);  
//strncpy(Inv_Data[which].dcc[1], "10.1", 3);  
//strncpy(Inv_Data[which].dcc[2], "1.2", 3); 
//strncpy(Inv_Data[which].dcc[3], "1.3", 3); 
//strncpy(Inv_Data[which].power[0], "233.0", 4); 
//strncpy(Inv_Data[which].power[1], "233.1", 4); 
//strncpy(Inv_Data[which].power[2], "33.2", 4);
//strncpy(Inv_Data[which].power[3], "33.3", 4);
//
//// lets see what we could get from the server    
//// set the array into a json object
//String json;
//int i = which ;
//    json="{";
//
//if(i==99) { 
//      i = iKeuze;
//      json += "\"sn\":\"" + String(Inv_Prop[i].invSerial) + "\"";
//      json += ",\"polled\":\"" + String(polled[i]) + "\"}";
//  } else {
//     json += "\"nm\":\"" + String(Inv_Prop[i].invLocation) + "\"";
//     json += ",\"polled\":\"" + String(polled[i]) + "\"";    
//     json += ",\"type\":\"" + String(Inv_Prop[i].invType) + "\"";
//     json += ",\"p0\":\"" + String(Inv_Data[i].power[0]) + "\""; 
//     json += ",\"p1\":\"" + String(Inv_Data[i].power[1]) + "\""; 
//     if(Inv_Prop[i].invType == 1 ){
//         json += ",\"p2\":\"" + String(Inv_Data[i].power[2]) + "\"";
//         json += ",\"p3\":\"" + String(Inv_Data[i].power[3]) + "\"";
//     }
//     json += ",\"state\":\"" + String(zigbeeUp) + "\"";
//     uint8_t night; 
//     if(!timeRetrieved ) { night=0; } else {
//     if(now() > switchonTime && now() < switchoffTime ) 
//         { night = 0; } else { night = 1; } 
//     }
//     json += ",\"sleep\":\"" + String(night) + "\"";
//     json += "}";     
//    }
//    Serial.println("get.Power reaction = " + json);
//
//}
