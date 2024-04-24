

void handle_data(httpd_req_t *req) 
{
// this handles all data requests from the server or other webpages
// first decide for what the request is, we do this with the argument

// first read the request into a char
  consoleOut("we are in handle_data 1");
  char*  buf;
  size_t buf_len;
  buf_len = httpd_req_get_url_query_len(req) + 1;
//  consoleOut("req =" + String(req));
  
  if (buf_len > 1) 
  {
     consoleOut("buf_len = " + String(buf_len) );
     delay(500);
     buf = (char*)malloc(buf_len);
     consoleOut("handle_data 2");
     if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
     consoleOut("handle_params querystring copied");  

    // here we place the filters to decide what is requested
    // the uri looks like getData?knop, getData?sldr=123   
    
    if(strstr(buf, "sldr")) {
       consoleOut("slider update");
       char param[QUERYKEY_MAX_LEN] = {0};
    
      if (httpd_query_key_value(buf, "sldr", param, sizeof(param)) == ESP_OK) duty = atoi(param);
      consoleOut("Found URL query parameter sldr");
      consoleOut("param = " + String(param));
      //duty = duty * 100; // String(param).toInt() ?? from 0 to 8192 (2^13)
      //the slider goes to 82 so this could be 8200, //this makes the light go out so
      inschakeltijdstip = now()-10;
      if(duty == 0) ledState = 0; else ledState = 1;
      set_pwm(duty);
      consoleOut("duty cycle set to " + String( ledc_get_duty(LEDC_MODE, LEDC_CHANNEL) ) ); 
      eventSend(0); //update the webpage
      httpd_resp_send(req, NULL, 0);

      } else    
    if(strstr(buf, "knop")) {
    // link = getData?knop
    consoleOut("knop update");
     //if(ledc_get_duty(LEDC_MODE,LEDC_CHANNEL) > 0) ledState = true; //(aan)
     consoleOut("getData?knop dvName = " + String(dvName));
     String json = "{";
     json += "\"name\":\"" + String(dvName) + "\",";
     json += "\"state\":\"" + String(ledState) + "\",";
     json += "\"duty\":\"" + String(ledc_get_duty(LEDC_MODE,LEDC_CHANNEL)) + "\"";
     json += "}";
     httpd_resp_send(req, json.c_str(), HTTPD_RESP_USE_STRLEN);
     json = String(); 
     }
    }
  } 
free(buf);
}
