#!/bin/sh

# take nginx as an example
# Get yesterday's date as YYYY-MM-DD
YESTERDAY=$(date -d 'yesterday' '+%Y-%m-%d')

# move log
mv /path/to/access.log /destination/access-$YESTERDAY.log
mv /path/to/error.log /destination/error-$YESTERDAY.log

PID_FILE=/usr/local/nginx/logs/nginx.pid

# Tell nginx to reopen the log file.
kill -USR1 $(cat $PID_FILE)

# crontab
# $sudo crontab -e
# 0   0   *   *   *   /usr/local/bin/rotate_log.sh 1>>/var/log/rotate_log.log 2>&1
# $sudo crontab -l