#include "CSVExporter.h"
#include <fstream>
#include <iostream>

std::string CSVExporter::escapeField(const std::string& field) {
    // Check if quoting is necessary
    if (field.find_first_of(",\"\n\r") == std::string::npos) {
        return field;
    }

    // Wrap in double-quotes; escape inner double-quotes by doubling them
    std::string result;
    result.reserve(field.size() + 2);
    result += '"';
    for (char c : field) {
        if (c == '"') result += '"';   // double the quote
        result += c;
    }
    result += '"';
    return result;
}

bool CSVExporter::exportToCSV(const std::vector<Book>& books,
                               const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "  [!!] Could not open file for writing: " << filename << "\n";
        return false;
    }

    // Header
    file << "ID,Title,Author,Year\n";

    // Data rows
    for (const auto& book : books) {
        file << book.id                      << ","
             << escapeField(book.title)      << ","
             << escapeField(book.author)     << ","
             << book.year                    << "\n";
    }

    file.close();
    std::cout << "  [OK] Exported " << books.size()
              << " record(s) to \"" << filename << "\"\n";
    return true;
}
