UPDATE users
SET name=$2, surname=$3, patronomic=$4
WHERE id=(SELECT id FROM users WHERE table_id=$1 ORDER BY ID DESC LIMIT 1);