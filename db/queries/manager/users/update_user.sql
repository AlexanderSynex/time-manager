INSERT INTO users(table_id, name, surname, patronymic)
VALUES($1, $2, $3, $4)
ON CONFLICT DO UPDATE;