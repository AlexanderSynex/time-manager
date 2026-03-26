SELECT token, login
FROM auth_schema.tokens
WHERE token = CAST($1 AS TEXT);