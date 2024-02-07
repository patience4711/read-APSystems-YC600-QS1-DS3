
const char ECU_HOMEPAGE [] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8' name='viewport' content='width=device-width, initial-scale=1'><title>ESP-ECU</title><link rel="icon" type="image/x-icon" href="/favicon.ico" /><link rel='stylesheet' type='text/css' href='/STYLESHEET_HOME'><style>body { background-color: #EEE;}span {padding: 6px;}table, th, td {border: 2px solid blue; font-size:16px; padding:6px; text-align:center; border-collapse:collapse;backgound-color:#dfff80;}tr {background-color:#ccffcc;}td { width:70px; }.btn { background-color: #199319; color: white; padding: 5px 22px; border-radius:6px;}.btn:hover {background: #eeeF; color:black;} @media only screen and (max-width: 800px) {th, td { width:60px; font-size:11px;}tr {height:35px;} .btn { padding: 5px 18px; font-size:10px;}}</style>
<script type="text/javascript" src="SECURITY"></script>
<script type='text/javascript'> 
var term,table_row;window.addEventListener("visibilitychange",(()=>{"visible"===document.visibilityState&&(getGeneral(),setTimeout(getAll,300))}));var cnt=0,totalEn=0;function loadScript(){getGeneral(),setTimeout(getAll,300)}function celbgc(e){e.startsWith("e")||e.startsWith("i")?document.getElementById(e).style="background-color:#c6ff1a":document.getElementById(e).style="background-color:#a6a6a6"}function getAll(){totalEn=0;for(let e=0;e<cnt;e++)term="get.Power?inv="+e,table_row="inv"+e,getData(e)}function getData(e){var t=new XMLHttpRequest;t.onreadystatechange=function(){if(4==this.readyState&&200==this.status){var t=this.responseText,n=JSON.parse(t),l=n.eN,o="r"+String(e);document.getElementById(o).style.display="table-row";var a="i"+String(e);document.getElementById(a).innerHTML="<a href='/details?inv="+String(e)+"'><button class='btn'>"+String(e)+"</button</a>";for(let t=0;t<4;t++)a="p"+String(e)+t,"n/e"==n.p[t]?celbgc(a):"n/a"==n.p[t]?document.getElementById(a).innerHTML=n.p[t]:document.getElementById(a).innerHTML=n.p[t].toFixed(1);celbgc(a="e"+String(e)),"n/a"!=l?(cnt>1&&(totalEn+=parseFloat(l),celbgc("e9"),document.getElementById("r9").style.display="table-row",document.getElementById("e9").innerHTML=totalEn.toFixed(2)),document.getElementById(a).innerHTML=l.toFixed(2)):document.getElementById(a).innerHTML=l,document.getElementById("noOutput").style.display="none",document.getElementById("4channel").style.display="block";const d=(new Date).toString();document.getElementById("time").innerHTML=d.substr(16,8)}},t.open("GET",term,!0),t.send()}function getGeneral(){var e=new XMLHttpRequest;e.onreadystatechange=function(){if(4==this.readyState&&200==this.status){var e=this.responseText,t=JSON.parse(e);cnt=t.cnt,console.log("getGeneral cnt = "+cnt);var n="polling from "+t.ps+" to "+t.pe;document.getElementById("times").innerHTML=n;var l=t.st,o=t.sl;0==t.rm&&(document.getElementById("ml").style.display="block"),"11"==l?document.getElementById("busy").style.display="block":(document.getElementById("busy").style.display="none",document.getElementById("failed").style.display="none"),"0"!=l&&"25"!=l||(document.getElementById("failed").style.display="block");var a=document.getElementById("maindiv");document.getElementById("sleep");"1"==o?(a.style.background="grey",document.getElementById("noOutput").style.display="none"):a.style.background="linear-gradient(#e8edc8, #c8eaed)"}},e.open("GET","get.General",!0),e.send()}if(window.EventSource){var source=new EventSource("/events");source.addEventListener("open",(function(e){console.log("Events Connected")}),!1),source.addEventListener("error",(function(e){e.target.readyState!=EventSource.OPEN&&console.log("Events Disconnected")}),!1),source.addEventListener("message",(function(e){console.log("message",e.data),"getall"==e.data&&getAll(),"general"==e.data&&getGeneral(),"reload"==e.data&&(getGeneral(),setTimeout(getAll,300))}),!1)}
</script></head><body onload='loadScript()'><div id='msect'><ul><li id='ml' style='float:right; display:none'><a id='haha' href='/MENU'>menu</a></li></ul></div><div id='msect'><center><h3>ESP ECU</h3></div><div id='msect'><div class='divstijl' id='maindiv'><center><p><span id='times'></span></p><h4>POWER / ENERGY @ <span id='time'></span></h4><div id='noOutput' style='display:block'><h4 style='color:red'>waiting for output</h4></div><div id='4channel' style='display:none;'><center><table><tr style='Background-color:lightblue; font-weight:bold; text-align:center; border:4px solid black;'><td>inverter<td>panel 0<td>panel 1<td>panel 2<td>panel 3<td>kWh</td></tr><tr id='r0' style='display:none;'><td id='i0'></td><td id='p00'></td><td id='p01'></td><td id='p02'></td><td id='p03'></td><td id='e0'></td></tr><tr id='r1' style='display:none;'><td id='i1'></td><td id='p10'></td><td id='p11'></td><td id='p12'></td><td id='p13'></td><td id='e1'></td></tr><tr id='r2' style='display:none;'><td id='i2'></td><td id='p20'></td><td id='p21'></td><td id='p22'></td><td id='p23'></td><td id='e2'></td></tr><tr id='r3' style='display:none;'><td id='i3'></td><td id='p30'></td><td id='p31'></td><td id='p32'></td><td id='p33'></td><td id='e3'></td></tr><tr id='r4' style='display:none;'><td id='i4'></td><td id='p40'></td><td id='p41'></td><td id='p42'></td><td id='p43'></td><td id='e4'></td></tr><tr id='r5' style='display:none;'><td id='i5'></td><td id='p50'></td><td id='p51'></td><td id='p52'></td><td id='p53'></td><td id='e5'></td></tr><tr id='r6' style='display:none;'><td id='i6'></td><td id='p60'></td><td id='p61'></td><td id='p62'></td><td id='p63'></td><td id='e6'></td></tr><tr id='r7' style='display:none;'><td id='i7'></td><td id='p70'></td><td id='p71'></td><td id='p72'></td><td id='p73'></td><td id='e7'></td></tr><tr id='r8' style='display:none;'><td id='i8'></td><td id='p80'></td><td id='p81'></td><td id='p82'></td><td id='p83'></td><td id='e8'></td></tr><tr id='r9' style='display:none;'><td colspan='5' style='text-align:right;'>total of all inverters&nbsp;</td><td id='e9'></td></tr></table></div><p>Powered by Hansiart</p><div id='busy'><span style='color:red;'><h3>checking / initialyzing zigbee network...</h3></span><br></div><div id='failed' style='display:none;'><span style='color:red;'><h3>the zigbee module is not working.</h3></span><br></div></center></div></div></body></html>
)=====";




// this is the original javascript. Made compact via https://www.digitalocean.com/community/tools/minify
/*
const char JAVA_SCRIPT[] PROGMEM = R"=====(
window.addEventListener('visibilitychange', () =>{
    if (document.visibilityState === 'visible') {
    //window.location.reload();}
    getGeneral();
    setTimeout(getAll, 300);    
    }
})

var term;
var table_row;
var cnt = 0;
var totalEn = 0;

function loadScript() {
  getGeneral();
  //console.log("getGeneral done, now getAll()");
  setTimeout(getAll, 300);
}
function celbgc(cel) {
  if(cel.startsWith("e") || cel.startsWith("i")) {
     document.getElementById(cel).style = "background-color:#c6ff1a"; } 
   else {document.getElementById(cel).style = "background-color:#a6a6a6";
  }
}

function getAll() {
  totalEn = 0;
  //console.log("getAll count = " + cnt);
  for (let i = 0; i < cnt; i++) {
  term = "get.Power?inv=" + i;
  table_row = "inv" + i;
  getData(i);
  }

}

function getData(invnr) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;
      //console.log("inverter = " + invnr);
      var obj = JSON.parse(antwoord);
      //cnt = obj.cnt;
      //console.log("cnt = " + cnt);
      var eN = obj.eN;
     
      var regel = "r" + String(invnr);
      document.getElementById(regel).style.display="table-row";
      var cel = "i" + String(invnr);
      //document.getElementById(cel).innerHTML = String(invnr);
      document.getElementById(cel).innerHTML = "<a href='/details?inv=" + String(invnr) + "'><button class='btn'>" + String(invnr) + "</button</a>";
      var cel;
      for (let z=0; z < 4 ; z++) {
        //console.log("obj.p[z] = " + obj.p[z]);
        cel = "p" + String(invnr) + z; 
        if (obj.p[z] == "n/e" ) {
            celbgc(cel);
          } 
          else 
        if (obj.p[z] == "n/a" ) {document.getElementById(cel).innerHTML = obj.p[z];}
          else {
          document.getElementById(cel).innerHTML = obj.p[z].toFixed(1);
        }
      }

      cel = "e" + String(invnr);
      celbgc(cel);
      
      if(eN != "n/a") {
          if(cnt > 1){
              totalEn = totalEn + parseFloat(eN);
              celbgc("e9"); 
              document.getElementById("r9").style.display="table-row";
              document.getElementById("e9").innerHTML = totalEn.toFixed(2);
              }
           document.getElementById(cel).innerHTML = eN.toFixed(2);
           } else {
           document.getElementById(cel).innerHTML = eN;
           } 
     
      document.getElementById("noOutput").style.display = "none";
      document.getElementById("4channel").style.display = "block";
      const d = new Date().toString();
      document.getElementById("time").innerHTML = d.substr(16,8);
    }
  }
  xhttp.open("GET", term, true);
  xhttp.send();
}

function getGeneral() {
//  console.log("term = " + term);
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;
//      console.log("answer = " + antwoord);
      var obj = JSON.parse(antwoord);
      cnt = obj.cnt;
      console.log("getGeneral cnt = " + cnt);
      var times = "polling from " + obj.ps + " to " + obj.pe;
      document.getElementById("times").innerHTML=times; 
      var st = obj.st;
      var sl = obj.sl;
      var rem = obj.rm;
      if(rem == 0) {document.getElementById("ml").style.display = "block";} // hide menu link          
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
    }
  }
  xhttp.open("GET", "get.General", true);
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
  if(e.data == "getall") {
  getAll();
  }
  if(e.data == "general") {
  getGeneral();
  }  
  if (e.data == "reload") {
  //window.location.reload(1);
    getGeneral();
    setTimeout(getAll, 300); 
  }
 }, false);
}

)=====";
*/
