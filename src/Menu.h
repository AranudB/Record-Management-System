/**
 * @file Menu.h
 * @brief CLI menu controller.
 *
 * Owns the interaction loop and delegates every operation to
 * BookRepository, InputValidator, and CSVExporter.
 * The controller itself never issues SQL.
 */
#pragma once

#include "BookRepository.h"
#include "InputValidator.h"
#include "CSVExporter.h"
#include <vector>
#include <string>

class Menu {
public:
    explicit Menu(BookRepository& repo);

    /** Start the main interaction loop (blocks until the user exits). */
    void run();

private:
    BookRepository& repo_;

    // ── Menu actions ─────────────────────────────────────────────────────
    void addRecord();
    void displayAllRecords();
    void searchById();
    void updateRecord();
    void deleteRecord();
    void sortRecords();
    void exportRecords();

    // ── Display utilities ────────────────────────────────────────────────
    void displayMainMenu()                            const;
    void printSection(const std::string& title)       const;
    void printBooks(const std::vector<Book>& books)   const;
};
