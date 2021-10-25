const char MENUPAGE[] PROGMEM = R"=====(
<style> li a { width: 130px; } 
@media only screen and (max-width: 800px) {
  li a {
    font-size:16px;
    padding: 14px 26px 10px 12px;
    width: 90px;
  }
}

</style>
<body>
<script type='text/javascript'>
function showSubmit() {
document.getElementById("submitknop").style.display = "block";
function helpfunctie() {
document.getElementById("help").style.display = "block";
}
function sl() {  
document.getElementById("help").style.display = "none";
}
}
</script>
<div id='msect'>
  <div id='help'>
  <span class='close' onclick='sl();'>&times;</span><h3>ECU HELP</h3>
  <b>CC2530 to ESP wiring</b><br>
  cc2530 -> nodemcu <br>
  p2   ->  d8<br>
  p3   ->  d7<br>
  rst  -> d5<br>
  vcc  -> 3.3V<br> 
  gnd  -> g
  <br><br>
  <b>cc2530 firmware</b><br>
  The cc2530 should be flashed with a firmware named "CC2530ZNP-with-SBL.hex".<br>
  You can do this with "ccloader" (google).<br><br>
  <b>night mode</b><br>
  Betweeen sunset and sunrise, there is no reaction from inverters.<br>
  You should not perform pairing or polling tasks then!<br><br>
  <b>troubleshooting</b><br>
  If the zigbee system failes, check the hardware/wiring and reboot.<br><br>
  </div>
</div
<div id='msect'>
  <ul>
  <li><a href='/'>home</a></li>
  </ul>
</div>
<br><br>
<div id='msect'>
  <kop>ESP ECU MENU</kop>
</div>



<div id='msect'>

  <div style='width: 30vw;'>
  <h4>&nbsp;&nbsp;ECU</h4>
  <section id="sectie">
  <ul style='width: 99%;'>
  <li><a href='/INFOPAGE'>info</a></li>
  <li><a href='/LOGPAGE'>system log</a></li>
  <li><a href='/INV_CONFIG'>inverters</a></li>
  <li><a href='/CONSOLE'>console</a></li>
  <li><a href='/BASISCONFIG'>settings</a></li>
  <li><a href='#' onclick='helpfunctie()'>help</a></li>
  </section>
  </ul><br>
  </div>
  <div style="width:20vw"></div>
  <div style='width: 30vw;'>
       <section id="sectie">
  <h4>&nbsp;&nbsp;SYSTEM</h4>
  <ul style='width:99%;'>
  <li><a href='/MQTT'>mosquitto</a></li>
  <li><a href='/GEOCONFIG'>time config</a></li>  
  <li><a href='/IPCONFIG'>ip config</a></li>
  <li><a href='/FWUPDATE'>fw update</a></li>
  <li><a onclick="return confirm('are you sure?')" href='/REBOOT'>reboot</a></li>
  <li><a onclick="return confirm('are you sure?')" href='/STARTAP'>start AP</a></li>
  </section>
  </ul><br>
  </div>
 
</div>
</body></html>
  )=====";
