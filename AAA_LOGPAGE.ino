#ifdef LOG

// ************************************************************************************
//                      U P D A T E    L O G
// ************************************************************************************
 void Update_Log(int what, char message[14]) {
  char nu[14];
  // when the log is full we start overwriting with the first row 
        sprintf(nu,"%d-%d:%d:%d ", day(), hour(), minute(), second());
        
        strcpy( Log_Events[logNr].date, nu );
                              
        Log_Events[logNr].kind = what;
        
        strcpy( Log_Events[logNr].message, message );

        logNr++;

        if (logNr >= Log_MaxEvents)
        {
            logNr = 0;//start again
            Log_MaxReached = true;
        }
}

String putList(const String& var)
{
//
  if(var == "rows") 
  {
    //Serial.println("found rows");
 
   char content[1536] = {0};
   char temp1[80]={0}; // 14 + 
   char temp2[8]={0};
   //char temp2[13];
   byte Log_Count = 0;
   Log_MaxReached ? Log_Count = Log_MaxEvents : Log_Count = logNr;  // determine if the max number of event is reached
   int j = logNr;
   // the rows 0-logNr are the recent updates, are printed from logNr to 0
   // so first we print the recent from logNr -> null (j=logNr) 
   // and next the old ones from maxnr -> logNr 
   for ( int i = 1; i <= Log_Count; i++ ) {
      j--; //  this is the index of the newest record in the array
      if (j ==-1) j = Log_MaxEvents - 1; // if we are below the first index of the array ,we start at the last
   
   switch ( Log_Events[j].kind ) {
      case 1:
         strncpy( temp2, "system\0",  7 ) ;
         break;
      case 2:
         strncpy( temp2, "zigbee\0",  7 ) ;
         break;
      case 3:
         strncpy( temp2, "mqtt\0",    5 ) ;
         break;
      case 4:
         strncpy( temp2, "pairing\0", 8 ) ;
        }  
      // One table line
      sprintf(temp1,"<tr><td>%s</td><td>%s</td><td>%s</td>", Log_Events[j].date, temp2, Log_Events[j].message );
      strcat(content, temp1);
      }
   return content;
  }
return String();
}

#endif
