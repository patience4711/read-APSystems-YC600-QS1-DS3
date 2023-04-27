const char DETAILSPAGE [] PROGMEM = R"=====(
<!DOCTYPE html><html><head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" type="text/css" href="/STYLESHEET">
<meta charset='utf-8'>
<link rel="icon" type="image/x-icon" href="/favicon.ico" />
<title>ESP-ECU</title>
<style>
table, th, td {border: 1px solid blue; font-size:16px; text-align:center} 
body {margin:10px 10px 0px 20px; font-family:'lato',Verdana,Sans-serif;font-size:16px;}
tr {height:35px;}
.pol (display:none;}
</style>
<script>
function loadScript() {
  loadData();
  setInterval(function ld() {
    //console.log("setting interval");  
    loadData();
  },90000);
}  
function loadData() {
  console.log("function loadData");
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;
      //console.log("answer = " + antwoord);
      var obj = JSON.parse(antwoord);

     // first display general data serial, shortid, type, 
      //var sid = obj.serial; 
      document.getElementById("ivn").innerHTML = obj.inv;
      document.getElementById("snr").innerHTML = obj.serial;
      var sid = obj.sid; //if 0x0000 not paired
      document.getElementById("sid").innerHTML = sid;
      var t = obj.type;
      var type ="YC600";
      if(t=="1"){type="QS1";}  if(t=="2") {type = "DS3";}
      document.getElementById("tp").innerHTML = type;      
      //console.log("sid = " + sid);
      if(sid != "0x0000" || sid == "") {
        
      var polled = obj.polled;
         if( polled=="1" ) { 
 
            document.getElementById("dcvc").style.display = "block"; 
            document.getElementById("npo").style.display = "none";

            document.getElementById("sq").innerHTML = obj.sq;
            document.getElementById("acv").innerHTML = obj.acv;
            document.getElementById("tmp").innerHTML = obj.temp;
            document.getElementById("fr").innerHTML = obj.freq;
            document.getElementById("v0").innerHTML = obj.dcv0;
            document.getElementById("v1").innerHTML = obj.dcv1;
            document.getElementById("v2").innerHTML = obj.dcv2;
            document.getElementById("v3").innerHTML = obj.dcv3;
            document.getElementById("c0").innerHTML = obj.dcc0;
            document.getElementById("c1").innerHTML = obj.dcc1;
            document.getElementById("c2").innerHTML = obj.dcc2;
            document.getElementById("c3").innerHTML = obj.dcc3;
                      
            } else {
            //display the not polled message
              document.getElementById("npo").style.display = "block";
              document.getElementById("dcvc").style.display = "none";
            } 
       } else {
        document.getElementById("npa").style.display = "block"; 
       }
     
    }    
  };
  xhttp.open("GET", "get.Inverter", true);
  xhttp.send();
}
</script>

</head><body onload='loadScript()'><center>
<h3>ESP-ECU INVERTER :&nbsp; <span id="ivn"></span></h3>
<table>
<colgroup><col span="1" style="background:#00cc66; width:100px;"><col span="1" style="background:#99ffcc; width:120px;"></colgroup>
<tr><td> Type <td id="tp">n/a</td></tr>
<tr><td> serialnr <td id="snr">n/a</td></tr>
<tr><td> ID <td id="sid">n/a</td></tr>
<tr><td>  signal quality <td id="sq">n/a</td></tr>
<tr><td>  ac voltage<td id="acv">n/a</td></tr>
<tr><td>  temperature<td id="tmp">n/a</td></tr>    
<tr><td>  frequency<td id="fr">n/a</td></tr>
</table>

<h4>INVERTER OUTPUT</h4>

<div id="dcvc" style="display:none;">
<table><tr style='background:#ccff33;'><td>unit</td><td>panel 0<td>panel 1<td>panel 2<td>panel 3</tr>
<colgroup><col span='1' style='background:#00cc66; width:80px;'><col span='4' style='background:#99ffcc; width:80px;'></colgroup>
<tr><td>dc voltage<td id="v0">n/a</td><td id="v1">n/a</td>
<td id="v2">n/a</td><td id="v3">n/a</td></tr>
<tr><td>dc voltage<td id="c0">n/a</td><td id="c1">n/a</td>
<td id="c2">n/a</td><td id="c3">n/a</td></tr>
</table></div>

<span id="npa" style="display:none;"><h2>this inverter is not paired!</h2></span>
<span id="npo" style="display:none;"><h2>this inverter is not polled!</h2></span>

<div style="position:fixed; text-align:center; width:100%;"><br><a href="/"><button style="width:150px; height:50px;text-align:center; font-weight:bold; font-size:20px; background:#db97b9;">close</button></a></div> 

</body></html>
)=====";
