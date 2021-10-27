<!DOCTYPE html>
<html lang="en" dir="ltr">
<head>
  <meta charset="utf-8">
  <link rel="stylesheet" href="style/bootstrap.css">
  <title>Tracker</title>
</head>
<body>
<!-- BASIC HTML MARKUP BEGIN-->
<?php include 'static/nav.php'; ?>
<center>
  <div class="container" id="risk" style="background-color : #00e600; padding : 20px; color : white; font-size : 20px;border-radius : 10px;">
    You are at a low risk of infection.
  </div>
  <br><hr>
  <h2 class="title">Disable screen sleep from here : </h2>
  <input class="btn btn-success" type="button" id="toggle" value="Screen sleep is enabled" />
<!-- this is if we implement point system
  <br><hr>
  <div class="container">
    <h2 class="title">Your quarantine points for today : </h2>
    <i><b>
      <div class="container" id="quarpnts">
        0
      </div>
    </b></i>
  </div>
-->
  <hr>
  <h2 class="title">Welcome, Your unique tracking id is : </h2>
  <br>
  <i><b>
    <div class="container" id="riderId">
      xxxx
    </div>
  </b></i>
  <hr>
  <h2>Your location : </h2>
  <div class="row">
    <div class="col-sm-6"style="box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);border-left-style : solid; border-left-color : red; border-left-width : 10px;">
      <h2>Latitude</h2>
      <b>
        <div class="" id="trackLat">
          0
        </div>
      </b>
    </div>
    <div class="col-sm-6" style="box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);border-left-style : solid; border-left-color : #ed4132; border-left-width : 10px;">
      <h2>Longitude</h2>
      <b>
        <div class="" id="trackLng">
          0
        </div>
      </b>
    </div>
  </div>
</center>
<!-- BASIC HTML MARKUP END -->

<!-- JAVASCRIPT CODE BEGIN -->

<script src="https://cdnjs.cloudflare.com/ajax/libs/nosleep/0.6.0/NoSleep.min.js" integrity="sha256-8gSB9ESo2/0hMlNuROxLSbZhQI6Enq/cMOPhiQO17bc=" crossorigin="anonymous"></script>
<script>
var noSleep = new NoSleep();
var wakeLockEnabled = false;
var toggleEl = document.querySelector("#toggle");
toggleEl.addEventListener('click', function() {
  if (!wakeLockEnabled) {
    alert('Screen sleep has been disabled. Screen will not turn off automatically');
    noSleep.enable(); // keep the screen on!
    wakeLockEnabled = true;
    toggleEl.value = "Screen sleep is disabled";
    document.body.style.backgroundColor = "#99e6ff";
  } else {
    noSleep.disable(); // let the screen turn off.
    wakeLockEnabled = false;
    toggleEl.value = "Screen sleep is enabled";
    document.body.style.backgroundColor = "";
  }
}, false);
</script>

<script type="text/javascript">
//COOKIES TO UNIQUELY INDENTIFY A USER
//SETTING COOKIES
function setCookie(cname, cdata, exdays) {
  var d = new Date();
  d.setTime(d.getTime() + (exdays*24*60*60*1000));
  var expiry = "expires=" + d.toUTCString();
  document.cookie = cname + "=" + cdata + ";" + expiry + ";path=/";
}
//GET COOKIE DATA
//https://stackoverflow.com/questions/10730362/get-cookie-by-name
function getCookie(cname) {
  var name = cname + "=";
  cookies = decodeURIComponent(document.cookie);
  cdata = cookies.split(";");
  for (var i =0; i<cdata.length; i++) {
    var c = cdata[i];
    while (c.charAt(0) == ' ') {
      c = c.substring(1);
    }
    if (c.indexOf(cname) == 0) {
      return c.substring(cname.length+1, c.length);
    }
  }
  return 0;
}
//generating a unique id for the user
//https://stackoverflow.com/questions/105034/how-to-create-guid-uuid
function uuidv4() {
  return ([1e7]+-1e3+-4e3+-8e3+-1e11).replace(/[018]/g, c =>
    (c ^ crypto.getRandomValues(new Uint8Array(1))[0] & 15 >> c / 4).toString(16)
  )
}
//function to check the risk of infection for the current user
function checkRisk() {
  if (getCookie('infected') == 1) {
    xhttp = new XMLHttpRequest();
    // xhttp.onreadystatechange = function() {
    //   if (this.readyState == 4 && this.status == 200){
        
    //   }
    // }
    document.getElementById('risk').style.backgroundColor = "#ff4d4d";
    document.getElementById('risk').innerHTML = 'You reported yourself infected. Please stay in self quarantine and avoid public places.';
    var riderId = getCookie('riderId');
    xhttp.open("GET", "ajaxify.php?rider_id=" + riderId + "&action=" + "inff",true);
    xhttp.send();
  } else {
    xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200){
        if (this.responseText.length > 0) {
          document.getElementById('risk').style.backgroundColor = "#ff4d4d";
          document.getElementById('risk').innerHTML = this.responseText;
        } else {
          document.getElementById('risk').style.backgroundColor = "#00e600";
          document.getElementById('risk').innerHTML = 'You are at a low risk of infection';
        }
      }
    }
    var riderId = getCookie('riderId');
    xhttp.open("GET", "ajaxify.php?rider_id=" + riderId + "&action=" + "checkrisk",true);
    xhttp.send();
  }
}

/* point based incentive

function quarpnts() {
  xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function(){
    if (this.readyState == 4 && this.status == 200){
      if (this.responseText != 0) {
        document.getElementById('quarpnts').innerHTML = this.responseText;
      }
    }
  }
  var riderId = getCookie('riderId');
  xhttp.open("GET", "ajaxify.php?rider_id=" + riderId + "&action=" + "quarpnts",true);
  xhttp.send();
}
*/
//CHECKING IF A COOKIE IS ASSIGNED IF NOT THEN SETTING riderId COOKIE
if (getCookie("riderIdSet") != 1) {
  setCookie("riderId", uuidv4(), 365);
  setCookie("riderIdSet", 1, 365);
}
document.getElementById('riderId').innerHTML = getCookie('riderId');
//TRACK IS THE TRACKING OBJECT
var track = {
  interval : 10000, //SET THE INTERVAL IN MILLISECONDS IN WHICH THE LOCATION WILL BE UPDATED
  update : function() {
    navigator.geolocation.getCurrentPosition(function (pos) {
        xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function(){
          if (this.readyState == 4 && this.status == 200){
            document.getElementById('trackLat').innerHTML = pos.coords.latitude.toFixed(7);
            document.getElementById('trackLng').innerHTML = pos.coords.longitude.toFixed(7);
          }
        }
        var riderId = getCookie('riderId');
        xhttp.open("GET", "ajaxify.php?rider_id=" + riderId + "&track_lng=" + pos.coords.longitude.toFixed(7) + "&track_lat=" + pos.coords.latitude.toFixed(7) + "&action=" + "log",true);
        xhttp.send();
    });
  }
};
function runAll() {
  //quarpnts();
  track.update();
  checkRisk();
}
//ONLOAD THIS FUNCTION IS CALLED
window.addEventListener("load", function(){
        if (navigator.geolocation) {
          track.update();
        } else {
          alert("GPS location is not supported");
        }
        //quarpnts();
      });
      checkRisk();  
      setInterval("runAll()", track.interval);
      // setTimeout(() => {
      //   location.reload();
      // }, track.interval);
</script>
<!-- JAVASCRIPT CODE END -->

</body>
</html>
