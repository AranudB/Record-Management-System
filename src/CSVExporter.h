/**
 * @file CSVExporter.h
 * @brief Exports a vector of Book records to a CSV file.
 *
 * Follows RFC 4180: fields containing commas, double-quotes or newlines
 * are wrapped in double-quotes, and inner double-quotes are doubled.
 */
#pragma once

#include "Book.h"
#include <vector>
#include <string>

class CSVExporter {
public:
    /**
     * Write books to filename as CSV with a header row.
     * @return true on success, false if the file could not be opened.
     */
    static bool exportToCSV(const std::vector<Book>& books,
                            const std::string& filename = "books_export.csv");

private:
    /** Wrap field in quotes if it contains a comma, quote, or newline. */
    static std::string escapeField(const std::string& field);
};
