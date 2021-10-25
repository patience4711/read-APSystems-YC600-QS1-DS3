
const char ECU_HOMEPAGE[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8' name="viewport" content="width=device-width, initial-scale=1">
<title>ESP-ECU</title>
<link rel="stylesheet" type="text/css" href="/STYLESHEET">  
<link rel="icon" type="image/x-icon" href="/favicon.ico" />

<style>
body {
 background-color: #EEE;
}
table, th, td {
  border: 1px solid blue; font-size:14px; padding:6px;
  }
#ch0, #ch1, #ch2, #ch3, #kwh { 
  width:78px;
  font-size:20px;
  text-align: center;
  }
#nameField {
  font-weight:bold; 
  font-size:20px'
}
@media only screen and (max-width: 800px) {
#ch0, #ch1, #ch2, #ch3, #kwh { 
  width:50px;
  font-size:16px;
  }
}
</style>
<script type="text/javascript" src="JAVASCRIPT"></script>
</head>

<body onload='loadScript()'>
<div id='msect'>
  <ul>
  <li style='float:right;'><a id='haha' href='/MENU'>menu</a></li>
  </ul>
</div>
<div id='msect'>
  <kop>HANSIART ESP ECU</kop>
</div>
<div id='msect'>
    <div class='divstijl' id='maindiv'>
        <center>
        <table style = "border:none;"><tr><td style = "border:none;">
        <select class='sb1' id="SEL#" onchange='setKeuze()'>
        <option value='0'>inverter 0</option>
        <option value='1'>inverter 1</option>
        <option value='2'>inverter 2</option>
        <option value='3'>inverter 3</option>
        <option value='4'>inverter 4</option>
        <option value='5'>inverter 5</option>
        <option value='6'>inverter 6</option>
        <option value='7'>inverter 7</option>
        <option value='8'>inverter 8</option>
        </select>
        <td style = 'border:none;' id='nameField'></table>
        <h4>INVERTER OUTPUT (WATTS) PER PANEL</h4>
        
        <div id='noOutput' style='display:block'>
          <h4 style='color:red'>waiting for output</h4>
        </div>
        
        <div id="4channel" style='display:none;'>
          <center><table>
          <tr style='Background-color:lightblue; font-weight:bold; font-size:14px; text-align:center;'><td>PANEL 1<td>PANEL 2<td>PANEL 3<td>PANEL 4</td></tr>
          <tr>
          <td><input id="ch0"></input><td><input id="ch1"></input>
          <td><input id="ch2"></input><td><input id="ch3"></input>
          <tr><td colspan="4" style="text-align: center;">
            inverter generated today :&ensp;<input id="kwh"></input>&ensp;kWh 
          </td></tr>
        </table>
        </div>  
        <br>
        <div id="busy">
            <br><br><span style='color:red;'><h3>Checking / initialyzing the zigbee radio...</h3></span><br>
        </div>
        <div id="failed" style="display:none;">
            <br><br><span style='color:red;'><h3>The cc2530 is not working, check the log.</h3></span><br>
        </div>
        <div id="sleep" style="display:none;">
            <br><br><span style='color:red;'><h3>No inverter polling until sunrise!</h3></span><br>
        </div>        
        </center>
    </div>
</div></body>

</html>
)=====";

const char JAVA_SCRIPT[] PROGMEM = R"=====(

var term;
function loadScript() {
setKeuze();
loadData();
}

function show4() {
document.getElementById("noOutput").style.display = "none";
document.getElementById("4channel").style.display = "block";
}
function show0() {
document.getElementById("noOutput").style.display = "block";
document.getElementById("4channel").style.display = "none";
}

function setKeuze() {
var a = document.getElementById("SEL#").value;
term = "get.Power?inv=" + a; 
getData();
}

setInterval(function loadData() {
  getData();
},15000);

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;
      var obj = JSON.parse(antwoord);
      var polled = obj.polled;
      var Type = obj.type;
      var nm = obj.nm
      document.getElementById("nameField").innerHTML = nm;
      var p0 = obj.p0;
      var p1 = obj.p1;
      var p2 = obj.p2;
      var p3 = obj.p3;
      var st = obj.state;
      var sl = obj.sleep;
      var eN = obj.eN;
      var ch0 = obj.ch0
      var ch1 = obj.ch1
      var ch2 = obj.ch2
      var ch3 = obj.ch3                  
      if(st == "11") {
        document.getElementById("busy").style.display = "block";        
      } else {
        document.getElementById("busy").style.display = "none";
      }
      if(st == "0" || st == "25") {
       document.getElementById("failed").style.display = "block"; 
       }
       var main = document.getElementById("maindiv")
       var slp = document.getElementById("sleep")      
       if(sl == "1") {
         main.style.background="grey"; 
         slp.style.display = "block"; 
         document.getElementById("noOutput").style.display = "none";
       } else {
         main.style.background="linear-gradient(#e8edc8, #c8eaed)";
         slp.style.display = "none";       
       }
      var panel_0 =  document.getElementById("ch0");
      if (p0 != "n/a") {
      panel_0.style.color="black";
      } else {
      panel_0.style.color="red";  
      }
      panel_0.value=p0;

      var panel_1 =  document.getElementById("ch1");
      if (p1 != "n/a") {
      panel_1.style.color="black";
      } else {
      panel_1.style.color="red";  
      }
      panel_1.value=p1;
      var panel_2 =  document.getElementById("ch2");
      if (p2 != "n/a") {
      panel_2.style.color="black";
      } else {
      panel_2.style.color="red";  
      }
      panel_2.value=p2;
      var panel_3 =  document.getElementById("ch3");
      if (p3 != "n/a") {
      panel_3.style.color="black";
      } else {
      panel_3.style.color="red";  
      }
      panel_3.value=p3;      
      document.getElementById("kwh").value=eN;
      if(polled == "1") {
      show4();
      } else {
      show0();  
      }
    }
  }
  xhttp.open("GET", term, true);
  xhttp.send();
}

)=====";



void sendHomepage() {
    toSend = FPSTR(ECU_HOMEPAGE);
    toSend.replace("#ID",String(ECU_ID));
  }
