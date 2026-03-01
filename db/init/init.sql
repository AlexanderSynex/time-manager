
-- Таблицы данных
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    table_id INT UNIQUE NOT NULL CHECK (table_id > 0),
    name VARCHAR(255) NOT NULL CHECK (name != ''),
    surname VARCHAR(255) NOT NULL CHECK (surname != ''),
    patronymic VARCHAR(255)
);

CREATE TABLE department (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL CHECK (name != ''),
    leader_id INT NOT NULL,
    internal_id INT UNIQUE CHECK (internal_id > 0),
    FOREIGN KEY (leader_id) REFERENCES users(id)
);

-- Рабочее время
CREATE TABLE worktime (
    user_id INT NOT NUlL,
    FOREIGN KEY (user_id) REFERENCES users(id),
    day DATE NOT NULL DEFAULT CURRENT_DATE,
    arrived TIMESTAMPTZ NOT NULL,
    departed TIMESTAMPTZ
    CONSTRAINT time_check CHECK (departed > arrived)
);

-- Таблицы отношений
CREATE TABLE department_workers (
    id SERIAL PRIMARY KEY,
    department_id INT NOT NULL,
    worker_id INT NOT NULL,
    FOREIGN KEY (department_id) REFERENCES department(id),
    FOREIGN KEY (worker_id) REFERENCES users(id)
);

CREATE TABLE department_relations (
    id SERIAL PRIMARY KEY,
    department_id INT NOT NULL,
    parent_department_id INT NOT NULL,
    FOREIGN KEY (department_id) REFERENCES department(id),
    FOREIGN KEY (parent_department_id) REFERENCES department(id)
);
