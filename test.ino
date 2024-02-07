// this function sends a message on request via the console
void testMessage() {
char sendCmd[100];
char s_d[254]={0};
int len = strlen(txBuffer);

//DebugPrint("len of txBuffer :  "); DebugPrintln(String(txBuffer));

      for(int i=0; i<len; i++) 
      {
        sendCmd[i] = txBuffer[i+7];
      }
       
       //len at the begin and crc at the end done by sendZigbee()
       //now we send this command
       sendZB(sendCmd);

       //now read the answer
       char * inMessage = readZB(s_d);
      if (readCounter == 0) {
         if(diagNose) ws.textAll("no answer "); 
          return;
        }       
}


#ifdef TEST
void testDecode() {
  char inMessage[1024]={0};
// we always test inverter 0, depending on the type we test the right string
int type = Inv_Prop[0].invType; //
  polled[0] = true;
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
   } else { //yc600
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
mqttPoll(0);
}
#endif


// En is een getal of n/a
