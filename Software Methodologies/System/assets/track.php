<?php
class RiderTrack
{
//FUNCTION TO LOG OR REGISTER THE NEW POSITION OF A USER/RIDER
public function logRider($riderid, $track_lng, $track_lat) {
  $curr_time = date("m/d/Y H:i:s");
  $database = new DB;
  //CONNECTING TO THE DB
  $handle = $database->connectToDb();
  //PREPARING QUERY
  $query = "INSERT INTO rider_track (rider_id, track_time, track_lng, track_lat) VALUES ('{$riderid}', '{$curr_time}', '{$track_lng}', '{$track_lat}')";
  if ($handle->query($query)) {
    return 1;
  }
  else {
    return 0;
  }
}
//WHEN THE SERVER DETECTS A NEW POSITION IT USES THE LAST LOGGED POSITION AND COMPARES IT WITH THE CURRENT POSITION
//IN CASE THE TWO POSITIONS MISMATCH THE USER HAS MOVED FROM ONE LOCATION TO ANOTHER
//IN SUCH A CASE THE PREVIOUS LOG IS CLOSED AND A TIME DURATION IS ADDED TO IT
//REPRESENTING FOR HOW LONG A USER HAS BEEN AT THAT LOCATION +-5 MINUTES
//FOR MORE ACCURACY CHANGE THE INTERVAL PARAMETER IN index.html
public function updateLog($riderid, $track_lng, $track_lat) {
  $curr_time = date("m/d/Y H:i:s");
  $database = new DB;
  //CONNECTING TO THE DB
  $handle = $database->connectToDb();
  //PREPARING QUERY
  $query = "SELECT * FROM rider_track WHERE rider_id = '{$riderid}' ORDER BY track_time DESC";
  $result = $handle->query($query);
  if ($result->num_rows > 0) {
    $row = $result->fetch_array(MYSQLI_ASSOC);
    if (($track_lng == $row['track_lng']) && ($track_lat == $row['track_lat'])) {
      $track_time = strtotime($row['track_time']);
      //DURATION THE USER STAYED AT LAST LOCATION
      $track_duration = abs(strtotime($curr_time) - $track_time);
      $row['track_duration'] = $track_duration;
      $query = "UPDATE rider_track SET track_duration = '{$row['track_duration']}' WHERE track_lng = '{$row['track_lng']}' AND track_lat = '{$row['track_lat']}' AND rider_id = '{$riderid}' AND track_time='{$row['track_time']}'";
      $handle->query($query);
    }
    else {
      $query = "DELETE FROM rider_track WHERE rider_id = '{$riderid}'";
      $handle->query($query);
      $track_time = strtotime($row['track_time']);
      //DURATION THE USED STAYED AT LAST LOCATION
      $track_duration = abs(strtotime($curr_time) - $track_time);
      $query = "UPDATE rider_track SET track_duration = '{$track_duration}' WHERE track_lng = '{$row['track_lng']}' AND track_lat = '{$row['track_lat']}' AND rider_id = '{$riderid}' AND track_time='{$row['track_time']}'";
      $handle->query($query);
      $this->logRider($riderid, $track_lng, $track_lat);
      return 1;
    }
  }
  else {
    $this->logRider($riderid, $track_lng, $track_lat);
    return 0;
  }
}
}
?>
