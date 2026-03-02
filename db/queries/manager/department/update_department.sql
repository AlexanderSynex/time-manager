UPDATE department 
SET name = $2, leader_id = $3
WHERE id = $1;