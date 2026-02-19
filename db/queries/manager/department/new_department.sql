INSERT INTO department(name, leader_id)
VALUES($1, (SELECT id FROM users WHERE table_id = $2));
