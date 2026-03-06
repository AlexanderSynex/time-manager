SELECT departed FROM worktime 
WHERE user_id=$1 AND day=CAST($2 AS DATE);
