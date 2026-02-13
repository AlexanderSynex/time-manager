CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    table_id int,
    create_time DATE,
    name VARCHAR(255),
    surname VARCHAR(255),
    patronomic VARCHAR(255)
)

CREATE TABLE time (
    id int,
    marked DATE
);
