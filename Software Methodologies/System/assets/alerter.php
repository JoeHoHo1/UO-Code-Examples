<?php
class Alerter {
  //USING THE CONTACT OF INFECTED USER CHECK THE RISK OF INFECTION IN OTHERS
  public function tracePotentialInfection($inf_rider_id) {
    $database = new DB;
    $handle = $database->connectToDb();
    $query = "SELECT * FROM contact WHERE rider_id = '{$inf_rider_id}' ORDER BY contact_time DESC";
    $res = $handle->query($query);
    while ($row = $res->fetch_array(MYSQLI_ASSOC)) {
      $time = date("m/d/Y H:i:s");
      $curr_time = strtotime($time);
      $contact_time = strtotime($row['contact_time']);
      if (abs($curr_time - $contact_time) / (60*60*24) > INCUBATION_PERIOD) {
        break;
      }
      //NOTE : THIS VARIABLE IS NOW IN config.php file
      if ($row['distance'] < RISK_RADIUS) {
        $query = "SELECT * FROM risk WHERE rider_id = '{$row['rider_id']}' AND sec_rider_id = '{$inf_rider_id}'";
        $result = $handle->query($query);
        if ($result->num_rows == 0) {
          $query = "INSERT INTO risk (rider_id, sec_rider_id) VALUES ('{$row['sec_rider_id']}', '{$inf_rider_id}')";
          $handle->query($query);
        }
      }
    }
  }

  //CHECK THE RISK OF THE USER
  public function checkRisk($riderid) {
    $database = new DB;
    $handle = $database->connectToDb();
    $query = "SELECT * FROM risk WHERE rider_id = '{$riderid}'";
    $result = $handle->query($query);
    if ($result->num_rows == 0) {
      return 0;
    } else {
      $i = 0;
      while ($row = $result->fetch_array(MYSQLI_ASSOC)) {
        $inf[$i] = $row['sec_rider_id'];
        $i = $i + 1;
      }
      return $inf;
    }
  }

  //WORKS WHEN THE USER REPORTS THEY IS INFECTED
  //SAVES THE USER IN INFECTED TABLE AND FINDS THE POTENTIALLY INFECTED PEOPLE
  public function logInfection($riderid) {
    $this->tracePotentialInfection($riderid);
    $time = date("m/d/Y H:i:s");
    $database = new DB;
    $handle = $database->connectToDb();
  }
}
?>
