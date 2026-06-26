#include "Menu.h"
#include <iostream>
#include <string>
#include <limits>

Menu::Menu(BookRepository& repo) : repo_(repo) {}

// ── Utilities ─────────────────────────────────────────────────────────────────

void Menu::printSection(const std::string& title) const {
    const std::string bar(60, '=');
    std::cout << "\n" << bar << "\n"
              << "  " << title << "\n"
              << bar << "\n";
}

void Menu::printBooks(const std::vector<Book>& books) const {
    if (books.empty()) {
        std::cout << "  (no records found)\n";
        return;
    }
    Book::printTableHeader();
    for (const auto& b : books) b.printTableRow();
    const std::string sep(88, '-');
    std::cout << sep << "\n"
              << "  Total: " << books.size() << " record(s)\n";
}

void Menu::displayMainMenu() const {
    std::cout << "\n" << std::string(42, '-') << "\n"
              << "    LIBRARY RECORD MANAGEMENT SYSTEM\n"
              << std::string(42, '-') << "\n"
              << "  1. Add a book\n"
              << "  2. Display all books\n"
              << "  3. Search by ID\n"
              << "  4. Update a book\n"
              << "  5. Delete a book\n"
              << "  6. Sort records\n"
              << "  7. Export to CSV\n"
              << "  0. Exit\n"
              << std::string(42, '-') << "\n";
}

// ── Main loop ─────────────────────────────────────────────────────────────────

void Menu::run() {
    std::cout << "\n  Welcome to the Library Record Management System\n"
              << "  " << repo_.count() << " record(s) currently in the database.\n";

    int choice = -1;
    while (choice != 0) {
        displayMainMenu();
        choice = InputValidator::readInt("  Your choice: ", 0, 7);
        switch (choice) {
            case 1: addRecord();         break;
            case 2: displayAllRecords(); break;
            case 3: searchById();        break;
            case 4: updateRecord();      break;
            case 5: deleteRecord();      break;
            case 6: sortRecords();       break;
            case 7: exportRecords();     break;
            case 0:
                std::cout << "\n  Goodbye!\n\n";
                break;
        }
    }
}

// ── Actions ───────────────────────────────────────────────────────────────────

void Menu::addRecord() {
    printSection("ADD BOOK");
    const std::string title  = InputValidator::readString("  Title  : ");
    const std::string author = InputValidator::readString("  Author : ");
    const int         year   = InputValidator::readYear("  Year   : ");

    if (repo_.addBook(Book(0, title, author, year))) {
        std::cout << "  [OK] Book added successfully.\n";
    } else {
        std::cout << "  [!!] Failed to add book.\n";
    }
}

void Menu::displayAllRecords() {
    printSection("ALL BOOKS");
    printBooks(repo_.getAllBooks());
}

void Menu::searchById() {
    printSection("SEARCH BY ID");
    const int id = InputValidator::readId("  Enter ID: ");
    const auto result = repo_.findById(id);
    if (result.has_value()) {
        Book::printTableHeader();
        result->printTableRow();
        std::cout << std::string(88, '-') << "\n";
    } else {
        std::cout << "  [!] No book found with ID " << id << ".\n";
    }
}

void Menu::updateRecord() {
    printSection("UPDATE BOOK");
    const int id = InputValidator::readId("  Enter the ID of the book to update: ");

    const auto result = repo_.findById(id);
    if (!result.has_value()) {
        std::cout << "  [!] No book found with ID " << id << ".\n";
        return;
    }

    std::cout << "\n  Current record:\n";
    Book::printTableHeader();
    result->printTableRow();
    std::cout << std::string(88, '-') << "\n\n"
              << "  Enter new values. Press ENTER to keep the current value.\n\n";

    // Helper lambda: display current value as hint, return new or current
    auto readOptional = [](const std::string& label, const std::string& current) {
        std::cout << "  " << label << " [" << current << "]: ";
        std::string line;
        std::getline(std::cin, line);
        const auto first = line.find_first_not_of(" \t");
        return (first == std::string::npos) ? current : line.substr(first);
    };

    std::string newTitle  = readOptional("Title ", result->title);
    std::string newAuthor = readOptional("Author", result->author);

    // Year: keep if empty, validate if provided
    int newYear = result->year;
    std::cout << "  Year   [" << result->year << "]: ";
    std::string yearLine;
    std::getline(std::cin, yearLine);
    if (!yearLine.empty() && yearLine.find_first_not_of(" \t") != std::string::npos) {
        try {
            int y = std::stoi(yearLine);
            if (y >= 1450 && y <= 2100) {
                newYear = y;
            } else {
                std::cout << "  [!] Year out of range (1450–2100), keeping original.\n";
            }
        } catch (...) {
            std::cout << "  [!] Invalid year, keeping original.\n";
        }
    }

    if (!InputValidator::confirm("\n  Save changes?")) {
        std::cout << "  Update cancelled.\n";
        return;
    }

    if (repo_.updateBook(Book(id, newTitle, newAuthor, newYear))) {
        std::cout << "  [OK] Book updated successfully.\n";
    } else {
        std::cout << "  [!!] Failed to update book.\n";
    }
}

void Menu::deleteRecord() {
    printSection("DELETE BOOK");
    const int id = InputValidator::readId("  Enter the ID of the book to delete: ");

    const auto result = repo_.findById(id);
    if (!result.has_value()) {
        std::cout << "  [!] No book found with ID " << id << ".\n";
        return;
    }

    std::cout << "\n  Record to delete:\n";
    Book::printTableHeader();
    result->printTableRow();
    std::cout << std::string(88, '-') << "\n";

    if (!InputValidator::confirm("\n  Are you sure you want to permanently delete this record?")) {
        std::cout << "  Deletion cancelled.\n";
        return;
    }

    if (repo_.deleteBook(id)) {
        std::cout << "  [OK] Book deleted successfully.\n";
    } else {
        std::cout << "  [!!] Failed to delete book.\n";
    }
}

void Menu::sortRecords() {
    printSection("SORT RECORDS");
    std::cout << "  Sort by:\n"
              << "    1. Title  (A → Z)\n"
              << "    2. Year   (oldest first)\n"
              << "    3. Author (A → Z)\n";
    const int choice = InputValidator::readInt("  Choice: ", 1, 3);

    std::vector<Book> books;
    std::string       label;
    switch (choice) {
        case 1: books = repo_.sortByTitle();  label = "BOOKS SORTED BY TITLE";  break;
        case 2: books = repo_.sortByYear();   label = "BOOKS SORTED BY YEAR";   break;
        case 3: books = repo_.sortByAuthor(); label = "BOOKS SORTED BY AUTHOR"; break;
    }
    printSection(label);
    printBooks(books);
}

void Menu::exportRecords() {
    printSection("EXPORT TO CSV");

    const auto books = repo_.getAllBooks();
    if (books.empty()) {
        std::cout << "  [!] No records to export.\n";
        return;
    }

    std::string filename = "books_export.csv";
    std::cout << "  Output filename [" << filename << "]: ";
    std::string input;
    std::getline(std::cin, input);
    const auto first = input.find_first_not_of(" \t");
    if (first != std::string::npos) filename = input.substr(first);

    CSVExporter::exportToCSV(books, filename);
}
