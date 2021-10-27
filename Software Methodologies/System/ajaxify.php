<?php
include_once "assets" . DIRECTORY_SEPARATOR . "config.php";
include_once "assets" . DIRECTORY_SEPARATOR . "track.php";
include_once "assets" . DIRECTORY_SEPARATOR . "tracer.php";
include_once "assets" . DIRECTORY_SEPARATOR . "database.php";
//AJAX REQUEST IS RECEIVED AND PROCESSED HERE
$action = $_REQUEST['action'];
if ($action == 'log') {

  $rider = new RiderTrack;
  $riderid = $_REQUEST['rider_id'];
  $track_lng = $_REQUEST['track_lng'];
  $track_lat = $_REQUEST['track_lat'];
  $rider->updateLog($riderid, $track_lng, $track_lat);
  $tracer = new Tracer;
  $tracer->vicinityUsers($riderid);
} else if ($action =='report'){
  $riderid = $_REQUEST['rider_id'];
  $tracer = new Tracer;
  // $tracer->tracePotentialInfection($riderid);
  echo $tracer->logInfection($riderid);
} else if ($action == 'checkrisk') {
  $time = date("m/d/Y H:i:s");
  $epoch_time = strtotime($time);
  $riderid = $_REQUEST['rider_id'];
  $tracer = new Tracer;
  if ($inf = $tracer->checkRisk($riderid)) {
    $count = 0;
    $txt = '';
    // $txt = 'You are at a high risk of infection due to coming in contact with ';
    foreach ($inf as $record) {
      if ($epoch_time - $record[1] < INCUBATION_PERIOD*24*60*60) {
        $txt .= ' {' . $record[0] . '} ';
      }
    }
    if (strlen($txt) > 0) {
      $txt = 'You are at a high risk of infection due to coming in contact with ' . $txt;
    }
    // $tracer->tracePotentialInfection($riderid);
    echo $txt;
  } else {
    echo "";
  }
} else if ($action == 'quarpnts') {
  $riderid = $_REQUEST['rider_id'];
  $tracer = new Tracer;
  echo $tracer->quarpnts($riderid);
} else if ($action == 'unreport') {
  $riderid = $_REQUEST['rider_id'];
  $tracer = new Tracer;
  echo $tracer->unreport($riderid);
} else if ($action == 'inff') {
  $riderid = $_REQUEST['rider_id'];
  $tracer = new Tracer;
  $tracer->tracePotentialInfection($riderid);
}
?>
