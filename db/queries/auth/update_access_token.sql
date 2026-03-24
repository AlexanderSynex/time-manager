INSERT INTO auth_schema.tokens(login, token)
VALUES($1, $2) ON CONFLICT (login) DO
UPDATE
SET token = EXCLUDED.token,
    last_update = now();