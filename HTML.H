const char HTML_HEAD[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP-ECU</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" type="image/x-icon" href="/favicon.ico" />
<link rel="stylesheet" type="text/css" href="/STYLESHEET">
<script type='text/javascript'>
function helpfunctie() {
document.getElementById("help").style.display = "block";
}
function sl() {  
document.getElementById("help").style.display = "none";
}
function showSubmit() {
document.getElementById("sub").style.display = "block";
}
function submitFunction(a) {
document.getElementById("bo").innerHTML="<br>wait...<br>processing<br>your<br>request"; 
document.getElementById("bo").style.display="block"; 
document.getElementById('formulier').submit();
setTimeout(function(){window.location.href=a;}, 3000 ); 
}
</script>
</head>
)=====";

const char STYLESHEET[] PROGMEM = R"=====(
  #sectie li{float:none;}
body {
  background: linear-gradient(#e8f5f7, #caf5fb); 
  color:navy;
  font-family:'lato',Verdana,Sans-serif;
  font-size:16px;
  }
#help {
  background-color: #ffffff; 
  border: solid 2px; 
  display:none; 
  padding:4px;
  width:94vw;
} 

.divstijl {
  background: linear-gradient(#e8edc8, #c8eaed); 
  padding:0px 0px 0px 0px;
  width: 96%;
  height: 99%; 
  }
#msect {
  display: flex;
  flex-basis: 100%;
  align-items: center;
  margin: 0px 0px;
  width:96vw;
}
#msect:before,
#msect:after {
  content: "";
  flex-grow: 1;
  margin: 0px 0px;
}

kop {
  font-size: 4vw;
  font-weight:bold;
  padding-top: 1vh;
  padding-bottom: 1vh;
}

p {
  color: #006600;
  font-family: 'lato',Verdana,Sans-serif;
  font-size:14px;
  }
.bt{
  display:block;
  width:124px;
  height:44px;
  padding:6px;
  margin:6px;
  text-align:center;
  border-radius:6px;
  color:white;
  background:red;
  font-weight:bold;
  font-size:18px;
  text-decoration:none;
  }
.red{
    background:#b9b9c1;
    color:white;
    border:2px solid black;
  }
.rad {
    background:#cc99ff;
    color:white;
    border:2px solid black;
    }

input { font-size:24px; }
.inp1 { width:46px; }
.inp2 { width:62px; } 
.inp3 { width:76px; }
.inp4 { width:180px;} 
.inp5 { width:172px;}
.inp6 { width:230px;} 

option {
    direction: ltr;
}
.sb1 {
   font-size:22px;
   height:30px;
  } 
 
tr {height:45px;} 
input[type=checkbox] 
  {
    width:30px; 
    height:30px;
    }
ul {
  list-style-type: none;
  margin: 0 auto 0 auto;
  padding: 0;
  overflow: hidden;
  background-color: #333;
  border-radius:6px;
  width: 96vw;
}
li {
  float: left;
}
li a:hover {
  background-color: #ff0808 !important;
}

a:focus {background-color: red;}

b {
color: #;
font-size:18px;
font-weight:bold;
}
.close {
  color: red;
  float: right;
  font-size: 48px;
  font-weight: bold;
  cursor: pointer;
}
.close:hover {color: green;}

th { width:44px; }
#bo {
  width:100%;
  height:100%; /* height of 100% too*/
  position: absolute;
  left:0; top:0;
  background-color:rgba(192,192,192,0.9);
  color:white; 
  z-index:10;
  font-size: 90px;
  font-weight: bold;
  text-align: center;
  display: none;
}
#sub { float: right; background:green; display:none }

li a {
  display: inline-block;
  color: white;
  text-align: center;
  padding: 14px 6px 10px;
  text-decoration: none;
  width:70px;
  font-size:20px;  
}

@media only screen and (max-width: 800px) {
  li a {
    font-size:16px;
    padding: 14px 26px 10px 12px;
    width: 50px;
  }
}
  
)=====";


//<script type='text/javascript'>setTimeout(function(){ window.location.href='/SW=BACK';}, 3000 ); </script>

const char CONFIRM[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP-ECU</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" type="text/css" href="/STYLESHEET">
</head>
<body>
<script type='text/javascript'>setTimeout(function(){ 
  window.location.href='/SW=BACK';}, 3000 ); </script>

<center><kop>PROCESSING YOUR REQUEST...</kop>
<br><br>
<div class='divstijl'>
Please wait while your request is being executed.<br><br>
If applicable, the data is saved and a reboot follows.<br><br>
Visit the log page or information page for more information<br><br>
This page will close automatically when the command is executed.<br>

<br><br>
<br>You will then return to the homepage. Check if the settings are correct.</center></div>
</body></html>
)=====";

const char CONFIRM_IP[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP-ECU</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" type="text/css" href="/STYLESHEET">
</head>
<body style='font-size:16px;'>
<script type='text/javascript'>setTimeout(function(){ window.location.href='http://{adres0}/'; }, 6000 ); </script>
<center>
<h1>SAVE IP SETTINGS</h1><div class='lijnstijl'></div><br><br><div class='divstijl'>
The IP settings are saved. <br><br>
Your Esp device is going to reboot. <strong>Wait util the led has blinked 3 times!</strong><br><br>
</body></html>
)=====";

const char CONFIRM_INV[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP-ECU</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" type="text/css" href="/STYLESHEET">
</head>
<body style='font-size:16px;'>
<script type='text/javascript'>setTimeout(function(){ window.location.href='/INV_CONFIG';}, 6000 ); </script>
<center>
<h1>PROCESSING THE INVERTER DATA...</h1><div class='lijnstijl'></div><br><br>
<div class='divstijl'>
The inverterdata are saved.<br><br>
In a moment the device is going to reboot.<br><br> 
You will be redirected to the inverter config page. 

<br><br>
</div>
<br><br> <a class='bt rad' style='background:green;font-size:22px' href='/SW=BACK'>OK</a></body></html>
)=====";


const char UPDATE_FORM[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<link rel="stylesheet" type="text/css" href="/STYLESHEET">
<style> 
input {
  font-size:3vw;
}  
</style>
<script>
function showDiv() {
document.getElementById('main').style.background="#b3ffd9";  
document.getElementById('hiddendiv').innerHTML="<marquee><kop>updating firmware, do not interrupt .....</kop></marquee>";
}
</script></head>
<body><div id='main'>
<center><kop> HANSIART FIRMWARE UPDATER</kop>
<br><br><br><table><tr><td>
<form method='POST' action='/handleFwupdate' enctype='multipart/form-data' onsubmit='showDiv()'>
<input id='inP' type='file' name='update' pattern='\S+' title='cannot be empty' required>
<tr><td><br><tr><td><input type='submit' value='update'></form>
<tr><td><br><tr><td><a href='/SW=BACK'><input type='submit' value='cancel'></table><br>
<div id='hiddendiv' style='margin: 50px; width: 40%; background:#e7f2bd;'></div><br><br>
</div></center></body></html>    
)=====";

const char WAIT_PAIR[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
</head><body onload="loadScript()">
<center><br><br><h2>PAIRING INVERTER {#}</h2>
<br><br><br>
<div id="marq" style='margin: 50px; width: 40%; background:#e7f2bd;'>
<marquee><h3>pairing, do not interrupt .....</h3></marquee>
</div>
<div id="hiddenSucc" style="display:none"<br><br><br>
<h3>The pairing succeeded, check the log.
<br><br><br><a href="/LOGPAGE">click here</a></h3>
</div>
<div id="hiddenFail" style="display:none"<br><br><br>
<h3>The pairing failed, check the log.
<br><br><br><a href="/LOGPAGE">click here</a></h3>
</div>

<script>
function loadScript() {
  getData();
  loadData();
}

setInterval(function loadData() {
 getData()
},9000);

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;
      var obj = JSON.parse(antwoord);
      var invID = obj.invID;
      
      if( invID != "1x1111" ) {
          if( invID == "0x0000" ) {
            document.getElementById("marq").style.display = "none";
            document.getElementById("hiddenFail").style.display = "block";
            setTimeout(function(){ window.location.href='/'; }, 5000 );
          } else {
            document.getElementById("marq").style.display = "none";
            document.getElementById("hiddenSucc").style.display = "block";
            setTimeout(function(){ window.location.href='/'; }, 5000 );
          }
       }
    }
  }
  xhttp.open("GET", "get.Paired", true);
  xhttp.send();
}
</script>
</body></html>    
)=====";


#define FAVICON_len 318
const uint8_t FAVICON[] PROGMEM = {
0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x10, 0x10, 0x00, 0x01, 0x00, 0x04, 0x00, 0x28, 0x01,
0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00,
0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2E, 0xDB,
0xD6, 0x00, 0xD1, 0x0F, 0x18, 0x00, 0xB4, 0xDD, 0xDE, 0x00, 0x66, 0xE1, 0xE3, 0x00, 0xD3, 0xE6,
0xE5, 0x00, 0x81, 0xE1, 0xE3, 0x00, 0x9A, 0xDA, 0xDB, 0x00, 0x75, 0xDF, 0xE0, 0x00, 0xB5, 0x14,
0x9D, 0x00, 0xB6, 0xE3, 0xE2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x44,
0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x49, 0x91, 0x99, 0x91, 0x19, 0x99, 0x19, 0x94, 0x49, 0x21,
0x22, 0x12, 0x21, 0x22, 0x12, 0x94, 0x49, 0x21, 0x66, 0x16, 0x61, 0x66, 0x12, 0x94, 0x49, 0x21,
0x55, 0x15, 0x51, 0x55, 0x12, 0x94, 0x49, 0x21, 0x57, 0x71, 0x17, 0x71, 0x11, 0x94, 0x49, 0x26,
0x50, 0x00, 0x00, 0x75, 0x62, 0x94, 0x49, 0x26, 0x57, 0x33, 0x30, 0x75, 0x62, 0x94, 0x48, 0x86,
0x58, 0x83, 0x80, 0x75, 0x62, 0x84, 0x48, 0x86, 0x58, 0x80, 0x88, 0x75, 0x68, 0x84, 0x48, 0x86,
0x58, 0x87, 0x88, 0x88, 0x88, 0x84, 0x48, 0x88, 0x88, 0x85, 0x58, 0x85, 0x88, 0x94, 0x48, 0x86,
0x68, 0x86, 0x66, 0x82, 0x82, 0x94, 0x48, 0x82, 0x28, 0x82, 0x22, 0x88, 0x82, 0x94, 0x48, 0x89,
0x98, 0x89, 0x99, 0x98, 0x99, 0x94, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
