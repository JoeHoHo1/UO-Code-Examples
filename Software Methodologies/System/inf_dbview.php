<!DOCTYPE html>
<html lang="en" dir="ltr">
<head>
  <meta charset="utf-8">
  <link rel="stylesheet" href="style/bootstrap.css">
  <title>Tracker</title>
<style>
table, th, td {
    border: 1px solid black;
}
</style>
</head>
<body>
<!-- BASIC HTML MARKUP BEGIN-->
  <?php include 'static/db_nav.php'; ?>

  <h1 class="title">Infected Users</h1>

  <!-- Printing the list of users  -->

  <?php
    include_once "assets" . DIRECTORY_SEPARATOR . "database.php";
    include_once "assets" . DIRECTORY_SEPARATOR . "config.php";
    $database = new DB;
    $handle = $database->connectToDb();

	$query="SELECT * FROM infected";
    $result = $handle->query($query);

	if ($result->num_rows > 0) {
      echo "<table style='width: 100%;border: solid 1px black;'>";
      echo"<tr><th>RIDER_ID</th><th>INF_TIME</th></tr>";

      while($row=$result->fetch_assoc())
      {
        echo "<tr><td>" . $row["rider_id"]. "</td><td>" . $row["inf_time"]. "</td></tr>";
	  }      
      echo "</table>";
	} else {
      echo "0 results";
	}

  
  $database->close();
   ?>

</body>
</html>
<!-- BASIC HTML MARKUP END -->
