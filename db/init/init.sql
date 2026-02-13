CREATE TABLE users (
    user_id SERIAL PRIMARY KEY,
    table_id int,
    create_time DATE,
    name VARCHAR(255),
    surname VARCHAR(255),
    patronomic VARCHAR(255)
)

CREATE TABLE time (
    user_id INT,
    marked DATE
    FOREIGN KEY (user_id) REFERENCES users(user_id),
);

CREATE TABLE department (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255),
    leader_id INT,
    FOREIGN KEY (leader_id) REFERENCES users(user_id)
)
