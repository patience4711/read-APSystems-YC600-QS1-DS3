const char HOMEPAGE[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP32 SWITCH</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link href="data:image/x-icon;base64,AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAu29YA0Q8YALTd3gBm4eMA0+blAIHh4wCa2tsAdd/gALUUnQC24+IAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAARERERERERERJkZmRGZkZlEkhIhIhIhKUSSFmFmFmEpRJIVUVUVUSlEkhV3EXcRGUSSZQAAB1YpRJJlczMHVilEiGWIOAdWKESIZYgIh1aIRIhliHiIiIhEiIiIVYhYiUSIZohmaCgpRIgiiCIoiClEiJmImZmJmUREREREREREQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" rel="icon" type="image/x-icon"/>
<link rel="stylesheet" type="text/css" href="/SHOWHTML?style">
<style>
#demoLPM:hover {
  cursor: pointer;
  background-color: yellow;
  font-size: 20px;
}
.bt {
  width:30vw;
  height:26vh;
  padding:6px;
  margin:6px;
  text-align:center;
  border-radius:15px;
  color:white;
  background:#262626;
  font-weight:bold;
  font-size:4vw;
  box-shadow: 0 9px #999;
  }

.bt::before{
  content: "SWITCH ON";
}
.bt_on{
  background:yellow;
  color:black;
  box-shadow: 0px 0px 20px 15px red;
  }
.bt_on::before{
  content:"SWITCH OFF";
}
.bt:active, .bt2:active {
  background-color: #3e8e41;
  box-shadow: 0 5px #666;
  transform: translateY(4px);
}
.bt2 {
  font-size:16px;
  padding: 10px;
  margin:6px;
  text-align:center;
  border-radius:8px;
  color:white;
  background: #8742f5;
  box-shadow: 0 6px #302442;
  }


#name {
  
}
@media only screen and (max-width: 800px) {
  .bt { height: 16vh;}
}
</style>
</head>
<body onload='loadScript()'>
<div id='msect'>
<ul>

<li style='float:right;'><a id='haha' href='/SHOWHTML?menu'>menu</a></li>
</ul>
</div>
<div id='msect'>
<br>
<kop><span id="NAME">HAIPS</span></kop>
<br></div>
<div id='msect'>
  <div class='divstijl' id = 'maindiv' style='height:70vh; background: grey;'>
  <center> <br><span id='pwdby'style='font-size:11px; margin:auto; display:table; color:white;'>powered by Hansiart</span><br>
  <button class='bt' id='knop' onclick='myFunction()'></button></center>
  <br>
  <br><br>

<center><input type="range" id="duty" name="DT" min="0" max="8192" value="{DT}" class="slider" onchange="sliderUpdate()" list="tickmarks">
<datalist id="tickmarks"><option>0</option><option>5</option><option>10</option><option>15</option><option>20</option></datalist>
<br><br><button class="bt2" onClick='savePWM()'>dim value : <span id="demoLPM"</span></button>
</div><br><br>
</div></body>

<script>

document.addEventListener("visibilitychange", function() {
    if (!document.hidden){
        console.log("Browser tab is visible")
        getData(); // update the page 
    } 
});

function savePWM() {
var pwm = document.getElementById('demoLPM').innerHTML;
if(!confirm("dimvalue = " + pwm + ", make this default?") ) {return};
var xhttp = new XMLHttpRequest();  
var url = "/parameters?pwmsave=" + pwm;
xhttp.open("GET", url, true);
xhttp.send();
}

var sldr = document.getElementById('duty');
var veld = document.getElementById('demoLPM');
// deze functie reageert op de sliderverandering

function sliderUpdate() {
  console.log("sliderUpdate");
  veld.style.color = "red";
//  veld.style.outline = "thick solid";  
  var outputLPM = document.getElementById("demoLPM");
  veld.innerHTML = sldr.value;
  sldr.oninput = function() { veld.innerHTML = this.value; }
  
  var xhttp = new XMLHttpRequest();  
  var sldrPos = document.getElementById("duty").value;
  console.log("sldrPos = " + sldrPos);
  var url = "getData?sldr=" + sldrPos;
  xhttp.open("GET", url, true);
  xhttp.send();
}


// deze functie reageert op de onclick van de knop
const btn = document.getElementById('knop');
function loadScript() {
// the first time we load the settings at once
getData();
}  
function myFunction() {
var xhttp = new XMLHttpRequest();  
    if(!btn.classList.contains('bt_on')) {
          btn.classList.add('bt_on');
          xhttp.open("GET", "/RUNACTION?sw=on", true);
          xhttp.send();
    } else {
          btn.classList.remove('bt_on'); 
          xhttp.open("GET", "/RUNACTION?sw=off", true);
          xhttp.send();
    }
    //getData();
}

// request is done on notification of a change
function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;  
      var obj = JSON.parse(antwoord);
      var state = obj.state;
      document.getElementById("duty").value=obj.duty;
      document.getElementById("demoLPM").innerHTML=obj.duty;
      document.getElementById("NAME").innerHTML=obj.name;

      if (state == "0") {
        btn.classList.remove('bt_on');
      } else {
        btn.classList.add('bt_on');
      }
    
    }
  };
  xhttp.open("GET", "getData?knop", true);
  xhttp.send();
}

if (!!window.EventSource) {
 var source = new EventSource('/events');

 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);

 source.addEventListener('message', function(e) {
  console.log("message", e.data);
  if(e.data == "getdata") {
     getData();
  }

 }, false);

}

</script>
</html>
)=====";
