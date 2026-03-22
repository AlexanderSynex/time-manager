INSERT INTO company_schema.users_info(table_id, name, surname, patronymic)
VALUES($1, $2, $3, $4) ON CONFLICT (table_id) DO
UPDATE
SET name = EXCLUDED.name,
    surname = EXCLUDED.surname,
    patronymic = EXCLUDED.patronymic;