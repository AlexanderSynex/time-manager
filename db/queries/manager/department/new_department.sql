INSERT INTO department (name, leader_id)
VALUES ($1, $2)
ON CONFLICT (name)
DO UPDATE SET
    name = EXCLUDED.name,
    leader_id = EXCLUDED.leader_id
RETURNING id;
