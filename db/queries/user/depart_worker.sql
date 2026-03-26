UPDATE company_schema.worktime 
SET departed=CURRENT_TIMESTAMP
WHERE user_id=$1 and day = CAST(CURRENT_DATE AS DATE) and departed is NULL
RETURNING departed;