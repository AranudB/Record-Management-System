/**
 * @file Book.h
 * @brief Book record data model.
 *
 * A Book holds the four core fields that define a library record:
 *   id | title | author | year
 */
#pragma once

#include <string>
#include <iostream>
#include <iomanip>

class Book {
public:
    int         id;
    std::string title;
    std::string author;
    int         year;

    // ── Constructors ────────────────────────────────────────────────────
    Book();
    Book(int id, const std::string& title, const std::string& author, int year);

    // ── Display helpers ─────────────────────────────────────────────────

    /** Print a separator + column header row. */
    static void printTableHeader();

    /** Print this book as one table row. */
    void printTableRow() const;
};
