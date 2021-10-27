<?php
class DB {
  public function connectToDb() {
      $handle = new mysqli(DB_HOST, DB_USER, DB_PASSWORD, DB_NAME);
      if ($handle->connect_error) {
        die("Connection error");
      }
      else {
        return $handle;
      }
  }
}
?>
