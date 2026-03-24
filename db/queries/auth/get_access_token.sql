SELECT token FROM auth_schema.tokens
WHERE login=CAST($1 AS TEXT);