SELECT EXISTS (
  SELECT * 
  FROM company_schema.worktime w
  WHERE w.user_id=$1 AND w.day=$2::date 
)::INT;
