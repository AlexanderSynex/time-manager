INSERT INTO auth_schema.users(login, table_id, password_hash)
VALUES(CAST($1 AS TEXT), $1, $2) ON CONFLICT (login) DO
UPDATE
SET password_hash = EXCLUDED.password_hash;
