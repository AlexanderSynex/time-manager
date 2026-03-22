SELECT table_id
FROM auth_schema.users
WHERE login = CAST($1 AS TEXT)
  AND password_hash = CAST($2 AS TEXT);