
-- Таблицы данных
CREATE TABLE users (
    table_id BIGINT PRIMARY KEY UNIQUE NOT NULL CHECK (table_id > 0),
    name VARCHAR(255) NOT NULL CHECK (name != ''),
    surname VARCHAR(255) NOT NULL CHECK (surname != ''),
    patronymic VARCHAR(255)
);

CREATE TABLE department (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    name VARCHAR(255) UNIQUE NOT NULL CHECK (name != ''),
    leader_id INT NOT NULL,
    internal_id INT UNIQUE CHECK (internal_id > 0),
    FOREIGN KEY (leader_id) REFERENCES users(table_id)
);

-- Рабочее время
CREATE TABLE worktime (
    user_id INT NOT NUlL,
    day DATE NOT NULL DEFAULT CURRENT_DATE,
    arrived TIMESTAMPTZ NOT NULL,
    departed TIMESTAMPTZ DEFAULT NULL,
    FOREIGN KEY (user_id) REFERENCES users(table_id),
    CONSTRAINT time_check CHECK (departed > arrived),
    CONSTRAINT unique_worker_date UNIQUE (user_id, day)
);

-- Таблицы отношений
CREATE TABLE department_workers (
    id SERIAL PRIMARY KEY,
    department_id INT NOT NULL,
    worker_id INT NOT NULL,
    FOREIGN KEY (department_id) REFERENCES department(id),
    FOREIGN KEY (worker_id) REFERENCES users(table_id)
);

CREATE TABLE department_relations (
    id GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    department_id INT NOT NULL,
    parent_department_id INT NOT NULL,
    FOREIGN KEY (department_id) REFERENCES department(id),
    FOREIGN KEY (parent_department_id) REFERENCES department(id)
);
