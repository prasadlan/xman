DROP TABLE IF EXISTS xman_extensions;
CREATE TABLE xman_extensions (
    name varchar(255) NOT NULL,
    weight int(11) NOT NULL,
    schema_version int(11) NOT NULL,
    projects text,
    PRIMARY KEY (name)
);
