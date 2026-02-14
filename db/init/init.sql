CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    table_id INT UNIQUE NOT NULL,
    name VARCHAR(255) NOT NULL,
    surname VARCHAR(255) NOT NULL,
    patronomic VARCHAR(255)
);

CREATE TABLE worktime (
    user_id INT,
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    day DATE NOT NULL DEFAULT CURRENT_DATE,
    arrived TIMESTAMPTZ,
    departed TIMESTAMPTZ
);

CREATE TABLE department (
    id SERIAL PRIMARY KEY,
    department_id INT NOT NULL,
    name VARCHAR(255),
    leader_id INT NOT NULL,
    FOREIGN KEY (leader_id) REFERENCES users(user_id)
);

CREATE TABLE department_relations (
    id SERIAL PRIMARY KEY,
    department_id INT NOT NULL,
    parent_department_id INT NOT NULL,
    FOREIGN KEY (department_id) REFERENCES department(id),
    FOREIGN KEY (parent_department_id) REFERENCES department(id)
)

CREATE TABLE department_workers (
    id SERIAL PRIMARY KEY,
    department_id INT NOT NULL,
    worker_id INT NOT NULL,
    FOREIGN KEY (department_id) REFERENCES department(id),
    FOREIGN KEY (worker_id) REFERENCES users(id)
)
