SELECT arrived FROM company_schema.worktime 
WHERE user_id=$1 AND day=$2::date;