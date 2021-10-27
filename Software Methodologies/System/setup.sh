#!/bin/bash

##########################################################
###                                                    ###
### This script allows the user to setup a MySQL       ###
### on University of Oregon's ix-dev server.           ###
###                                                    ###
##########################################################

########## VALUES ##########

PORT=3777
PASSWORD='guest'

########### CODE ###########

# Did the user supply enough arguments to this script?
if [ $# -ne 2 ]; then
  echo "Please use as \"$0 <user> <home dir>\""
  echo "user: If you ssh using ngr@ix.cs.uoregon.edu, user should be \"ngr\""
  echo "home dir: Your home directory"
  echo "            - This can easily be found through \`cd ~\` then \`pwd\`"
  exit 1
fi

USER=$1
HOMEPATH=$2

# Ask if user really wants to follow through with running script
echo "This program will stop the services of any existing MySQL databases. If
$HOMEPATH/mysql-data exists, this will be moved to mysql-data-old in
the same path."
echo -n "Do you wish to proceed? [y / n] "
read userInput;
if [[ $userInput = 'n' ]] || [[ $userInput = 'N' ]]; then
	echo "Exiting...";
	exit 1;
elif ! [[ $userInput = 'y' ]] && ! [[ $userInput = 'Y' ]]; then
	echo "Please respond with y or n. Exiting...";
	exit 1;
fi

if [[ -d $HOMEPATH/mysql-data-old ]]; then
  echo -n "Are you sure? $HOMEPATH/mysql-data-old already exists and will be overwritten. [y / n] "
  read userInput;
  if [[ $userInput = 'n' ]] || [[ $userInput = 'N' ]]; then
    echo "Exiting...";
    exit 1;
  elif ! [[ $userInput = 'y' ]] && ! [[ $userInput = 'Y' ]]; then
    echo "Please respond with y or n. Exiting...";
    exit 1;
  fi
  rm -rf $HOMEPATH/mysql-data-old
fi

# Make sure script is ran from directory /path/to/ssd (probably ~/ssd)
if [[ -z `ls | grep "sql"` ]]; then
  if [[ -z `ls sql | grep "rider_track.sql"` ]]; then
    echo "Please restart script from the directory /path/to/ssd"
    exit 1
  fi
fi

# Confirm that "mysqlctl" is accessible from current directory
if [[ -z `echo $PATH | grep "/local/bin"` ]]; then
	export PATH=$PATH:/local/bin
fi

# Allow mysqlctl to run the "install" command
if [ -d "$HOMEPATH/mysql-data" ]; then
  mysqlctl stop > /dev/null
  PIDS=(`pidof mysql` `pidof mysqld` `pidof mysqlctl`)
  for PID in $PIDS; do
    if [ -z ${PID+x} ]; then
      disown $PID
      kill -9 $PID
    fi
  done
  mv $HOMEPATH/mysql-data $HOMEPATH/mysql-data-old
  sleep 5
fi

# Re-install the server, and prompt the user to put a pre-defined password
printf "\n*** NOTE: Use password \"%s\" when prompted ***\n" "$PASSWORD"
sleep 2
mysqlctl install

# Change premade MySQL config file, config variables for PHP, and DB variables
cp sql/init_my.cnf ./my.cnf
cp assets/init_config.php assets/config.php
sed -i 's|HOMEPATH|'$HOMEPATH'|g' ./my.cnf
sed -i 's|PORT|'$PORT'|g' ./my.cnf
sed -i 's|CHANGEME|'$USER'|g' assets/config.php
sed -i 's|PORT|'$PORT'|g' assets/config.php
mv my.cnf ~/.my.cnf

# Start the new MySQL server
mysqlctl start

# Make sure it is running on port $PORT
if [[ -z `mysqlctl status | grep "$PORT"` ]]; then
  echo "[ERROR] MySql is not using port $PORT."
  echo "Please refer to the POTENTIAL ERRORS section of the Installation Guide."
  exit 1
fi

sleep 5

# Load up schemas
mysql -p < sql/rider_track.sql

echo "Setup Complete! If no errors occurred, the DB is now functional."
