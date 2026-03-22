#!/bin/bash

psql -U "${POSTGRES_USER}" -d "${POSTGRES_DB}" <<-EOSQL
CREATE EXTENSION IF NOT EXISTS pgcrypto;
INSERT INTO auth_schema.users(login, table_id, password_hash, scopes)
VALUES ('admin', 0000, encode(digest('$ADMIN_PASSWORD', 'sha1'), 'hex'), ARRAY['admin', 'user'])
ON CONFLICT (login) DO UPDATE SET
  password_hash = EXCLUDED.password_hash;
EOSQL