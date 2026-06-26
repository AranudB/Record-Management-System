-- ============================================================
--  Record Management System – Database Schema
--  Run once to initialise the database:
--    mysql -u root -p < sql/schema.sql
-- ============================================================

CREATE DATABASE IF NOT EXISTS library_db
    CHARACTER SET  utf8mb4
    COLLATE        utf8mb4_unicode_ci;

USE library_db;

CREATE TABLE IF NOT EXISTS books (
    id         INT          NOT NULL AUTO_INCREMENT,
    title      VARCHAR(255) NOT NULL,
    author     VARCHAR(255) NOT NULL,
    year       INT          NOT NULL,
    created_at TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP
                                     ON UPDATE CURRENT_TIMESTAMP,

    PRIMARY KEY (id),

    -- Indexes for the sort / search operations used by the application
    INDEX idx_title  (title),
    INDEX idx_author (author),
    INDEX idx_year   (year)

) ENGINE = InnoDB
  DEFAULT CHARSET = utf8mb4;

-- ── Optional seed data (remove if not needed) ─────────────────────────────
INSERT IGNORE INTO books (id, title, author, year) VALUES
    (1, 'The C++ Programming Language', 'Bjarne Stroustrup', 1985),
    (2, 'Clean Code',                   'Robert C. Martin',  2008),
    (3, 'Design Patterns',              'Gang of Four',      1994),
    (4, 'The Pragmatic Programmer',     'Hunt & Thomas',     1999),
    (5, 'Introduction to Algorithms',  'Cormen et al.',     1990);
