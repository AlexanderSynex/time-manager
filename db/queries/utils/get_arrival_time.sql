SELECT arrived FROM worktime 
WHERE user_id=$1 AND day=$2::date;