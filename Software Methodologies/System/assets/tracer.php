<?php
class Tracer {
  function __construct() {
    $this->database = new DB;
    $this->handle = $this->database->connectToDb();
  }
  //CHECK IF TWO TIME RANGES OVERLAP
  private function checkTimeOverlap($s1, $s2, $e1, $e2) {
    $s1 = $s1 - INTERVAL;
    $e1 = $e1 + INTERVAL;
    if (max($s1, $s2) <= min($e1, $e2)) {
      return 1;
    }
    return 0;
  }

  // GIVES THE DISTANCE BETWEEM TWO COORDINATES IN METRES (BY DEFAULT)
  private function distanceCalculator($latitudeFrom, $longitudeFrom, $latitudeTo, $longitudeTo) {
    // To get the result in some other unit change the radius of the earth in that unit
    // Default is in metres
    //https://stackoverflow.com/questions/14750275/haversine-formula-with-php/30703064
    $earthRadius = 6371000;
    // convert from degrees to radians
    $latFrom = deg2rad($latitudeFrom);
    $lonFrom = deg2rad($longitudeFrom);
    $latTo = deg2rad($latitudeTo);
    $lonTo = deg2rad($longitudeTo);

    $latDelta = $latTo - $latFrom;
    $lonDelta = $lonTo - $lonFrom;

    $angle = 2 * asin(sqrt(pow(sin($latDelta / 2), 2) +
      cos($latFrom) * cos($latTo) * pow(sin($lonDelta / 2), 2)));
    return $angle * $earthRadius;
  }

  // FINDS THE PEOPLE WHO ARE IN THE VICINITY
  public function vicinityUsers($riderid) {
    $time = date("m/d/Y H:i:s");
    $database = new DB;
    $handle = $database->connectToDb();
    $query = "SELECT * FROM rider_track WHERE rider_id = '{$riderid}'";
    $result = $handle->query($query);
    $rider_log = $result->fetch_array(MYSQLI_ASSOC);
    $query = "SELECT * FROM rider_track WHERE rider_id != '{$riderid}'";
    $res = $handle->query($query);
    while($user_log = $res->fetch_array(MYSQLI_ASSOC)) {
     $s1 = strtotime($rider_log['track_time']);
     $e1 = strtotime($rider_log['track_time']) + $rider_log['track_duration'];
     $s2 = strtotime($user_log['track_time']);
     $e2 = strtotime($user_log['track_time']) + $user_log['track_duration'];
     if ($this->checkTimeOverlap($s1, $s2, $e1, $e2)) {
       $distance = $this->distanceCalculator($rider_log['track_lat'], $rider_log['track_lng'], $user_log['track_lat'], $user_log['track_lng']);
       $query = "SELECT * FROM contact WHERE rider_id = '{$riderid}' AND sec_rider_id = '{$user_log['rider_id']}' ORDER BY contact_time DESC";
       $result = $handle->query($query);
       if ($result->num_rows > 0) {
         $row = $result->fetch_array(MYSQLI_ASSOC);
         $curr_day = date('d');
         $log_day = date('d', strtotime($row['contact_time']));
         if ($curr_day == $log_day) {
           if ($row['distance'] >= $distance) {
             $query = "UPDATE contact SET distance = '{$distance}', contact_time = '{$time}' WHERE rider_id = '{$riderid}' AND sec_rider_id = '{$user_log['rider_id']}'";
             $handle->query($query);
             $query = "UPDATE contact SET distance = '{$distance}', contact_time = '{$time}' WHERE sec_rider_id = '{$riderid}' AND rider_id = '{$user_log['rider_id']}'";
             $handle->query($query);
           }
         } else {
           $query = "INSERT INTO contact (rider_id, sec_rider_id, distance, contact_time) VALUES ('{$riderid}', '{$user_log['rider_id']}','{$distance}', '{$time}')";
           $handle->query($query);
           $query = "INSERT INTO contact (rider_id, sec_rider_id, distance, contact_time) VALUES ('{$user_log['rider_id']}', '{$riderid}','{$distance}', '{$time}')";
           $handle->query($query);
         }
       } else {
         $query = "INSERT INTO contact (rider_id, sec_rider_id, distance, contact_time) VALUES ('{$riderid}', '{$user_log['rider_id']}','{$distance}', '{$time}')";
         $handle->query($query);
         $query = "INSERT INTO contact (rider_id, sec_rider_id, distance, contact_time) VALUES ('{$user_log['rider_id']}', '{$riderid}','{$distance}', '{$time}')";
         $handle->query($query);
       }
     }
    }
  }
  
  //USING THE CONTACT OF INFECTED USER CHECK THE RISK OF INFECTION IN OTHERS
  public function tracePotentialInfection($inf_rider_id) {
    $count = 0;
    $query = "SELECT * FROM contact WHERE rider_id = '{$inf_rider_id}' ORDER BY contact_time ASC";
    $res = $this->handle->query($query);
    while ($row = $res->fetch_array(MYSQLI_ASSOC)) {
      $time = date("m/d/Y H:i:s");
      $curr_time = strtotime($time);
      $contact_time = strtotime($row['contact_time']);
      // if (abs($curr_time - $contact_time) / (60*60*24) > INCUBATION_PERIOD) {
      //   continue;
      // }
      //NOTE : THIS VARIABLE IS NOW IN config.php file
      if ($row['distance'] < RISK_RADIUS) {
        $query = "SELECT * FROM risk WHERE rider_id = '{$row['sec_rider_id']}' AND sec_rider_id = '{$inf_rider_id}'";
        $result = $this->handle->query($query);
        if ($result->num_rows == 0) {
          $query = "INSERT INTO risk (rider_id, sec_rider_id, distance, r_time) VALUES ('{$row['sec_rider_id']}', '{$inf_rider_id}', '{$row['distance']}', '{$contact_time}')";
          $this->handle->query($query);
        } else {
          $query = "UPDATE risk SET r_time = '{$contact_time}', distance = '{$row['distance']}' WHERE rider_id = '{$row['sec_rider_id']}' AND sec_rider_id = '{$inf_rider_id}'";
          $this->handle->query($query);
        }
      }
    }
  }

  //CHECK THE RISK OF THE USER
  public function checkRisk($riderid) {
    $database = new DB;
    $handle = $database->connectToDb();
    $query = "SELECT * FROM risk WHERE rider_id = '{$riderid}' ORDER BY r_time";
    $result = $handle->query($query);
    if ($result->num_rows == 0) {
      return 0;
    } else {
      $i = 0;
      while ($row = $result->fetch_array(MYSQLI_ASSOC)) {
        $inf[$i][0] = $row['sec_rider_id'];
        $inf[$i][1] = $row['r_time'];
        $i = $i + 1;
      }
      return $inf;
    }
  }

  // WORKS WHEN THE USER REPORTS THEY IS INFECTED
  // SAVES THE USER IN INFECTED TABLE AND FINDS THE POTENTIALLY INFECTED PEOPLE
  public function logInfection($riderid) {
    $this->tracePotentialInfection($riderid);
    $time = date("m/d/Y H:i:s");
    $database = new DB;
    $handle = $database->connectToDb();
    $query = "SELECT * FROM infected WHERE rider_id = '{$riderid}'";
    $result = $handle->query($query);
    if ($result->num_rows > 0) {
      return 0;
    } else {
      $query = "INSERT INTO infected (rider_id, inf_time) VALUES ('{$riderid}', '{$time}')";
      return $handle->query($query);
    }
  }

  //FUNCTION TO ASSIGN POINTS BASED ON THE LEVEL OF QUARANTINE OF USER
  public function quarpnts($riderid) {
    $time = date('m/d/Y');
    $day = date("m/d/Y H:i:s", strtotime($time));
    $database = new DB;
    $handle = $database->connectToDb();
    $query = "SELECT * FROM contact WHERE rider_id = '{$riderid}' AND contact_time >= '{$day}'";
    $result = $handle->query($query);
    $distance = 10000000;
    $i = 0;
    while ($row = $result->fetch_array(MYSQLI_ASSOC)) {
      $i++;
      $distance = min($row['distance'], $distance);
    }
    $distance = $distance/100;
    if ($i == 0) {
      $avg = 1.57;
    }
    else {
      $avg = $distance/$i;
    }
    $magic = 6.36;
    $points = atan($avg) * $magic;
    return round($points);
  }
  public function unreport($riderid) {
    $database = new DB;
    $handle = $database->connectToDb();
    $query = "DELETE FROM infected WHERE rider_id = '{$riderid}'";
    $result = $handle->query($query);
    return $result;
  }
}
?>
