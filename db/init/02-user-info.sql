
CREATE SCHEMA IF NOT EXISTS company_schema;

-- Таблицы данных
CREATE TABLE IF NOT EXISTS company_schema.users_info (
    table_id BIGINT PRIMARY KEY,
    name VARCHAR(255),
    surname VARCHAR(255),
    patronymic VARCHAR(255),
    FOREIGN KEY (table_id) REFERENCES auth_schema.users(table_id)
);

-- Таблицы данных
CREATE TABLE IF NOT EXISTS company_schema.users_info (
    table_id BIGINT PRIMARY KEY,
    name VARCHAR(255),
    surname VARCHAR(255),
    patronymic VARCHAR(255),
    FOREIGN KEY (table_id) REFERENCES auth_schema.users(table_id)
);

-- Рабочее время
CREATE TABLE IF NOT EXISTS company_schema.worktime (
    user_id INT NOT NUlL,
    day DATE NOT NULL DEFAULT CURRENT_DATE,
    arrived TIMESTAMPTZ NOT NULL,
    departed TIMESTAMPTZ DEFAULT NULL,
    FOREIGN KEY (user_id) REFERENCES auth_schema.users(table_id),
    CONSTRAINT time_check CHECK (departed > arrived),
    CONSTRAINT unique_worker_date UNIQUE (user_id, day)
);
