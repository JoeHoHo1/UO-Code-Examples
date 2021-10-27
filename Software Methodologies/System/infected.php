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
  <h1 class="title">Are you infected with COVID19? Report here : </h1>
  <button type="button" class="btn btn-danger" name="button" style="padding:15px;" onclick="report()">Report yourself as infected</button>
  <br><br>
  <button type="button" class="btn btn-success" name="button" style="padding:15px;" onclick="unreport()">Report yourself as not infected</button>
  <br><br><hr>
  <h1 class="title">Following is the list of infected patients :</h1>

  <!-- Printing the list of infected patients  -->

  <?php
    include_once "assets" . DIRECTORY_SEPARATOR . "database.php";
    include_once "assets" . DIRECTORY_SEPARATOR . "config.php";
    $database = new DB;
    $handle = $database->connectToDb();
    $query = "SELECT * FROM infected";
    if ($result = $handle->query($query)) {
      while ($row = $result->fetch_array(MYSQLI_ASSOC)) {
        echo $row['rider_id'] . "<br>";
      }
    }
   ?>
</center>
<!-- BASIC HTML MARKUP END -->

<!-- JAVASCRIPT BEGINS  -->
<script type="text/javascript">
//SETTING COOKIES
function setCookie(cname, cdata, exdays) {
  var d = new Date();
  d.setTime(d.getTime() + (exdays*24*60*60*1000));
  var expiry = "expires=" + d.toUTCString();
  document.cookie = cname + "=" + cdata + ";" + expiry + ";path=/";
}
//GETTING COOKIE DATA
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
  function report() {
    xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function(){
      if (this.readyState == 4 && this.status == 200){
        if (this.responseText == 1) {
          alert('You have been reported as infected');
          setCookie('infected', 1, 365);
        } else if (this.responseText == 0) {
          alert('You are already reported as infected');
        }
      }
    }
    var riderId = getCookie('riderId');
    xhttp.open("GET", "ajaxify.php?rider_id=" + riderId + "&action=" + "report",true);
    xhttp.send();
  }

  function unreport() {
    xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function(){
      if (this.readyState == 4 && this.status == 200){
          alert('You have been reported as not infected');
          setCookie('infected', 0, 365);
      }
    }
    var riderId = getCookie('riderId');
    xhttp.open("GET", "ajaxify.php?rider_id=" + riderId + "&action=" + "unreport",true);
    xhttp.send();
  }
</script>
<!-- JAVASCRIPT ENDS  -->
</body>
</html>
