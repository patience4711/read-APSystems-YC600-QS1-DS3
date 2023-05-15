
const char ECU_HOMEPAGE[] PROGMEM = R"=====(
<!DOCTYPE html><html><head>
<meta http-equiv="refresh" content="112">
<meta charset='utf-8' name="viewport" content="width=device-width, initial-scale=1">
<title>ESP-ECU</title>
<link rel="stylesheet" type="text/css" href="/STYLESHEET">  
<link rel="icon" type="image/x-icon" href="/favicon.ico" />

<style>
body {
 background-color: #EEE;
}
span {
  padding: 6px;
}
table, th, td {
  border: 2px solid blue; font-size:16px; padding:6px; text-align:center; border-collapse:collapse;backgound-color:#dfff80;
  }
tr {background-color:#ccffcc;}
td { width:70px; }
.btn {
  background-color: #199319;
  color: white;
  padding: 5px 22px;
  border-radius:6px;
}

.btn:hover {background: #eeeF; color:black;}

@media only screen and (max-width: 800px) {
th, td { width:60px; font-size:12px; }
tr {height:35px;} 
.btn { padding: 5px 18px; font-size:10px;}
}
</style>

<script type="text/javascript" src="JAVASCRIPT"></script>
<script type="text/javascript" src="SECURITY"></script>
</head>

<body onload='loadScript()'>
<div id='msect'>
  <ul>
  <li><a href='/INFOPAGE'>info</a></li>
  <li><a href='/LOGPAGE'>log</a></li>
  <li id="ml" style='float:right; display:none'><a id='haha' href='/MENU'>menu</a></li>
  </ul>
</div>
<div id='msect'>
  <kop>HANSIART ESP ECU</kop>
</div>
<div id='msect'>
    <div class='divstijl' id='maindiv'>
        <center>
        
        <p>polling &nbsp; from  <span id='srt'></span> to <span id='sst'></span></p>
        <h4>POWER (W) AND TODAY'S ENERGY (kWh)</h4>
        
        <div id='noOutput' style='display:block'>
          <h4 style='color:red'>waiting for output</h4>
        </div>
        
        <div id="4channel" style='display:none;'>
          <center><table>
          <tr style='Background-color:lightblue; font-weight:bold; text-align:center; border:4px solid black;'>
          <td>INV<td>PANEL0<td>PANEL1<td>PANEL2<td>PANEL3<td>kWh</td></tr>
          <tr id="r0" style="display:none;"><td id="i0"></td><td id="p00"></td><td id="p01"></td><td id="p02"></td><td id="p03"></td><td id="e0"></td></tr>
          <tr id="r1" style="display:none;"><td id="i1"></td><td id="p10"></td><td id="p11"></td><td id="p12"></td><td id="p13"></td><td id="e1"></td></tr>
          <tr id="r2" style="display:none;"><td id="i2"></td><td id="p20"></td><td id="p21"></td><td id="p22"></td><td id="p23"></td><td id="e2"></td></tr>        
          <tr id="r3" style="display:none;"><td id="i3"></td><td id="p30"></td><td id="p31"></td><td id="p32"></td><td id="p33"></td><td id="e3"></td></tr>
          <tr id="r4" style="display:none;"><td id="i4"></td><td id="p40"></td><td id="p41"></td><td id="p42"></td><td id="p43"></td><td id="e4"></td></tr>
          <tr id="r5" style="display:none;"><td id="i5"></td><td id="p50"></td><td id="p51"></td><td id="p52"></td><td id="p53"></td><td id="e5"></td></tr>
          <tr id="r6" style="display:none;"><td id="i6"></td><td id="p60"></td><td id="p61"></td><td id="p62"></td><td id="p63"></td><td id="e6"></td></tr>
          <tr id="r7" style="display:none;"><td id="i7"></td><td id="p70"></td><td id="p71"></td><td id="p72"></td><td id="p73"></td><td id="e7"></td></tr>
          <tr id="r8" style="display:none;"><td id="i8"></td><td id="p80"></td><td id="p81"></td><td id="p82"></td><td id="p83"></td><td id="e8"></td></tr>
          <tr id="r9" style="display:none;"><td colspan="5" style="text-align:right;">total of all inverters&nbsp;</td><td id="e9"></td></tr>
        </table>
        </div>  
        
        <div id="busy">
            <span style="color:red;"><h3>checking / initialyzing zigbee network...</h3></span><br>
        </div>
        <div id="failed" style="display:none;">
            <span style='color:red;'><h3>the cc25xx is not working, check the log.</h3></span><br>
        </div>
      
        </center>
    </div>
</div></body>

</html>
)=====";

const char JAVA_SCRIPT[] PROGMEM = R"=====(

var term;
var table_row;
var cnt = 9;
var totalEn = 0;
window.addEventListener("beforeunload", function(event) {
  console.log("UNLOAD:1");
getTimes();
});

function loadScript() {
getTimes();
getAll();
}

function celbgc(cel) {
//background color of the cells 
if(cel.startsWith("e") || cel.startsWith("i")) {
   document.getElementById(cel).style = "background-color:#c6ff1a"; } 
// else if(cel.startsWith("i")) {
//   document.getElementById(cel).style = "background-color:#00cc44";}
 else {document.getElementById(cel).style = "background-color:#a6a6a6";
 }
}

setInterval(function loadData() {
getAll();
},30000);

function getAll() {
  totalEn = 0;
  for (let i = 0; i < cnt; i++) {
  term = "get.Power?inv=" + i;
  table_row = "inv" + i;
//  console.log("table_row = " + table_row); 
//  console.log("term = " + term); 
  getData(i);
  }
}

function getData(invnr) {
//  console.log("term = " + term);
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;
//      console.log("answer = " + antwoord);
      var obj = JSON.parse(antwoord);
      var nm = obj.nm;
      var polled = obj.polled;
      var Type = obj.type;
      
      var p0 = obj.p0;
      var p1 = obj.p1;
      var p2 = obj.p2;
      var p3 = obj.p3;
      var st = obj.state;
      var sl = obj.sleep;
      var eN = obj.eN;
      cnt = parseInt(obj.count);
      var rem = obj.remote;
      if(rem == 0) {document.getElementById("ml").style.display = "block";}           
      if(st == "11") {
        document.getElementById("busy").style.display = "block";
      } else {
        document.getElementById("busy").style.display = "none";
        document.getElementById("failed").style.display = "none";
      }
      if(st == "0" || st == "25") {
       document.getElementById("failed").style.display = "block";
       }
       var main = document.getElementById("maindiv")
       var slp = document.getElementById("sleep")      
       if(sl == "1") {
         main.style.background="grey"; 
         document.getElementById("noOutput").style.display = "none";
       } else {
         main.style.background="linear-gradient(#e8edc8, #c8eaed)";
       }
      if(nm == "N/A") {return 0;}
      
      var regel = "r" + String(invnr);
      document.getElementById(regel).style.display="table-row";
      var cel = "i" + String(invnr);
      document.getElementById(cel).innerHTML = "<a href='/details?inv=" + String(invnr) + "'><button class='btn'>" + String(invnr) + "</button</a>";
      //celbgc(cel);

      var cel = "p" + String(invnr) + "0";
      if (p0 != "n/e") {
       if (polled == "1") {
        document.getElementById(cel).innerHTML = p0; } 
        else 
        {
        document.getElementById(cel).innerHTML = "n/a";
        }      
      } 
      else 
      {
      celbgc(cel);
      }
 
      var cel = "p" + String(invnr) + "1";
      if (p1 != "n/e") {
      if (polled == "1") {
        document.getElementById(cel).innerHTML = p1; } 
        else 
        {
        document.getElementById(cel).innerHTML = "n/a";; 
        }      
      } 
      else 
      {
      //document.getElementById(cel).style = "background-color:#a3c2c2";
      celbgc(cel);
      }

      var cel = "p" + String(invnr) + "2";
      if (p2 != "n/e") {
      if (polled == "1") {
        document.getElementById(cel).innerHTML = p2; } 
        else 
        {
       document.getElementById(cel).innerHTML = "n/a"; 
        }      
      } 
      else 
      {
      celbgc(cel);
      }   

      var cel = "p" + String(invnr) + "3";
      if (p3 != "n/e") {
      if (polled == "1") {
        document.getElementById(cel).innerHTML = p3; } 
        else 
        {
        document.getElementById(cel).innerHTML = "n/a"; 
        }      
      } 
      else 
      {
      celbgc(cel);
      }  

      var cel = "e" + String(invnr);
      celbgc(cel);
      if (eN != "n/e") {
      if(polled == "1") {
            document.getElementById(cel).innerHTML = eN;
            if(cnt > 1){
              totalEn = totalEn + parseFloat(eN);
              celbgc("e9"); 
              document.getElementById("r9").style.display="table-row";
              document.getElementById("e9").innerHTML = totalEn.toFixed(2);
              }
         } else {
           document.getElementById(cel).innerHTML = "n/a"; 
         } 
      
      } else {
        document.getElementById(cel).innerHTML = "n/e";
      }
    
      document.getElementById("noOutput").style.display = "none";
      document.getElementById("4channel").style.display = "block";

    }
  }
  xhttp.open("GET", term, true);
  xhttp.send();
}

function getTimes() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;
      var obj = JSON.parse(antwoord);
      var srt = obj.srt;
      var sst = obj.sst;
      document.getElementById("srt").innerHTML = srt;
      document.getElementById("sst").innerHTML = sst;        
    }
  }
  xhttp.open("GET", "get.Times", true);
  xhttp.send();
}
)=====";


//void sendHomepage() {
//    toSend = FPSTR(ECU_HOMEPAGE);
//    toSend.replace("#ID",String(ECU_ID));
//  }
