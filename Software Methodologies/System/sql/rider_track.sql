CREATE DATABASE db;
USE db;

CREATE TABLE `rider_track` (
  `rider_id` varchar(120) NOT NULL,
  `track_time` varchar(20) NOT NULL DEFAULT 0,
  `track_lat` decimal(11,7) NOT NULL,
  `track_lng` decimal(11,7) NOT NULL,
  `track_duration` int(11) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `infected` (
  `rider_id` varchar(120) NOT NULL,
  `inf_time` varchar(20) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `contact` (
  `rider_id` varchar(120) NOT NULL,
  `sec_rider_id` varchar(120) NOT NULL,
  `distance` int(11) NOT NULL DEFAULT 0,
  `contact_time` varchar(20) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `risk` (
  `rider_id` varchar(120) NOT NULL,
  `sec_rider_id` varchar(120) NOT NULL,
  `distance` int(11) NOT NULL DEFAULT 0,
  `r_time` varchar(20) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

COMMIT;
