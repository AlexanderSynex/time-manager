UPDATE worktime 
SET departed=CURRENT_TIMESTAMP
WHERE user=$1;
