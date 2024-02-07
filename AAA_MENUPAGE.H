const char MENUPAGE[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP-ECU</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" type="image/x-icon" href="/favicon.ico" />
<link rel="stylesheet" type="text/css" href="/STYLESHEET_HOME">
<style> 
li a { width: 130px; } 
.list {width:70%; height:250px;}
@media only screen and (max-width: 800px) {
  li a {
    font-size:16px;
    padding: 14px 26px 10px 12px;
    width: 90px;
  }
.list {width:99%;}
}
</style>
<script type="text/javascript" src="SECURITY"></script>
<script> function cl() { window.location.href='/';} </script></head><body>
<div id='msect'>
  <ul>
  <li><a href='/LOGPAGE'>journal</a></li>
  <li id='fright'><span class='close' onclick='cl();'>&times;</span></ul>  
  </ul>
</div>
<br><br>
<div id='msect'><kop>ESP ECU MENU</kop></div><br><br><br>

<div id='msect'>
  <div style='width: 30vw;'>
  <ul class='list'>
  <li><a href='/ABOUT'>system info</a></li>
  <li><a href='/INV_CONFIG'>inverters</a></li>
  <li><a href='/CONSOLE'>console</a></li>
  <li><a href='/BASISCONFIG'>settings</a></li>
  <li><a href='/MQTT'>mosquitto</a></li>
  </section>
  </ul><br>
  </div>
  <div style="width:20vw"></div>
  <div style='width: 30vw;'>
 
  <ul class='list'>
  <li><a href='/GEOCONFIG'>time config</a></li>  
  <li><a href='/IPCONFIG'>ip config</a></li>
  <li><a href='/FWUPDATE'>fw update</a></li>
  <li><a onclick="return confirm('are you sure?')" href='/REBOOT'>reboot</a></li>
  <li><a onclick="return confirm('are you sure?')" href='/STARTAP'>start AP</a></li>
  </ul><br>
  </div>
 
</div>
</body></html>
  )=====";
