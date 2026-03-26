INSERT INTO company_schema.worktime (user_id, day, arrived)
VALUES($1, CURRENT_DATE, CURRENT_TIMESTAMP)
ON CONFLICT (user_id, day) DO NOTHING
RETURNING arrived;