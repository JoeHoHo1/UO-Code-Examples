#!/bin/bash

##########################################################
#                                                        #
#  This script allows the user to backup a MYSQL databse #
#  which could be imported back into the server.         #
#                                                        #
#  On the server side of MYSQL services, to restore:     #
#                                                        #
#  $mysql -h {hostname}\                                 #
#      -P {port number} \                                #
#      -u{username} \                                    #
#      -p{password} \                                    #
#      -{database_name} < {sql_dump}.sql                 #
#                                                        #
##########################################################

######################### VALUES #########################

# Modify these parameters for connection to MYSQL.

HOST='ix.cs.uoregon.edu'
PORT=3769
PASSWORD='guest'
DATABASE_NAME='db'

BACKUP_FOLDER='db_backup'

########################## PATH ###########################

# This is for creating a new directory to store backups if
# not exist.

mkdir -p ${BACKUP_FOLDER}

########################## CODE ###########################

# This will create a MYSQL dump file for backup every 6
# hours. The name of backup file will be based on 
# "backup_Y-M-D_H.sql"

while :
do
    TIME=$(date +"%F_%H")
    mysqldump -p${PASSWORD} ${DATABASE_NAME} > ${BACKUP_FOLDER}/backup_${TIME}.sql

    printf "\nBackup Created ${FOLDER}/backup_${TIME}.sql\n"
    printf "\nIf run in terminal without \'&\', use CTRL+C to stop the script at any time.\n"
    echo "Otherwise, note the PID of the process and kill this process using $ kill -9 <PID> at any time."
    sleep 6h
done
