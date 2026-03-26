SELECT u.table_id
FROM auth_schema.users u
JOIN auth_schema.tokens t ON u.login = t.login 
WHERE t.token=CAST($1 AS TEXT)