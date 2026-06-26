#include "Book.h"

// ── Constructors ─────────────────────────────────────────────────────────────

Book::Book() : id(0), year(0) {}

Book::Book(int id, const std::string& title, const std::string& author, int year)
    : id(id), title(title), author(author), year(year) {}

// ── Display helpers ───────────────────────────────────────────────────────────

static const int COL_ID     = 6;
static const int COL_TITLE  = 38;
static const int COL_AUTHOR = 28;
static const int COL_YEAR   = 6;
static const int TABLE_W    = COL_ID + COL_TITLE + COL_AUTHOR + COL_YEAR + 10;

void Book::printTableHeader() {
    std::string sep(TABLE_W, '-');
    std::cout << sep << "\n"
              << std::left
              << "| " << std::setw(COL_ID)     << "ID"
              << "| " << std::setw(COL_TITLE)   << "TITLE"
              << "| " << std::setw(COL_AUTHOR)  << "AUTHOR"
              << "| " << std::setw(COL_YEAR)    << "YEAR"
              << " |\n"
              << sep << "\n";
}

void Book::printTableRow() const {
    // Truncate long fields for display
    auto trunc = [](const std::string& s, int n) {
        return s.size() > static_cast<size_t>(n)
               ? s.substr(0, n - 1) + "…"
               : s;
    };

    std::cout << std::left
              << "| " << std::setw(COL_ID)    << id
              << "| " << std::setw(COL_TITLE)  << trunc(title, COL_TITLE)
              << "| " << std::setw(COL_AUTHOR) << trunc(author, COL_AUTHOR)
              << "| " << std::setw(COL_YEAR)   << year
              << " |\n";
}
