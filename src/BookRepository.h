/**
 * @file BookRepository.h
 * @brief Data-access layer for Book records.
 *
 * All SQL lives here; the rest of the code never touches raw queries.
 * Uses Database::escapeString() on every user-supplied string to prevent
 * SQL injection.
 */
#pragma once

#include "Book.h"
#include "Database.h"
#include <vector>
#include <optional>

class BookRepository {
public:
    explicit BookRepository(Database& db);

    // ── CRUD ────────────────────────────────────────────────────────────
    bool                addBook(const Book& book);
    std::vector<Book>   getAllBooks()         const;
    std::optional<Book> findById(int id)      const;
    bool                updateBook(const Book& book);
    bool                deleteBook(int id);

    // ── Queries ─────────────────────────────────────────────────────────
    bool              idExists(int id)    const;
    int               count()             const;

    // ── Sorted views ────────────────────────────────────────────────────
    std::vector<Book> sortByTitle()  const;
    std::vector<Book> sortByYear()   const;
    std::vector<Book> sortByAuthor() const;

private:
    Database& db_;

    /** Execute *sql* (a SELECT) and materialise the rows into a Book vector. */
    std::vector<Book> fetchBooks(const std::string& sql) const;
};
