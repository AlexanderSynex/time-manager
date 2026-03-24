CREATE SCHEMA IF NOT EXISTS auth_schema;

CREATE TABLE IF NOT EXISTS auth_schema.users (
  login TEXT UNIQUE PRIMARY KEY NOT NULL CHECK (TRIM(login) = login),
  table_id BIGINT UNIQUE NOT NULL CHECK (table_id >= 0),
  password_hash TEXT,
  scopes TEXT[] NOT NULL DEFAULT ARRAY['user'],
  CONSTRAINT non_empty_scopes CHECK (array_length(scopes, 1) > 0)
);

CREATE TABLE IF NOT EXISTS auth_schema.tokens (
  token TEXT UNIQUE NOT NULL ,
  login TEXT UNIQUE,
  last_update TIMESTAMPTZ NOT NULL DEFAULT now(),
  FOREIGN KEY (login) REFERENCES auth_schema.users(login)
);
