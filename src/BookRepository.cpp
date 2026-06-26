#include "BookRepository.h"
#include <sstream>
#include <stdexcept>

BookRepository::BookRepository(Database& db) : db_(db) {}

// ── Private helper ────────────────────────────────────────────────────────────

std::vector<Book> BookRepository::fetchBooks(const std::string& sql) const {
    std::vector<Book> books;
    MYSQL_RES* res = db_.query(sql);
    if (!res) return books;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr) {
        books.emplace_back(
            std::stoi(row[0]),          // id
            row[1] ? row[1] : "",       // title
            row[2] ? row[2] : "",       // author
            std::stoi(row[3])           // year
        );
    }
    mysql_free_result(res);
    return books;
}

// ── CRUD ──────────────────────────────────────────────────────────────────────

bool BookRepository::addBook(const Book& book) {
    std::ostringstream sql;
    sql << "INSERT INTO books (title, author, year) VALUES ('"
        << db_.escapeString(book.title)  << "', '"
        << db_.escapeString(book.author) << "', "
        << book.year                     << ")";
    return db_.execute(sql.str());
}

std::vector<Book> BookRepository::getAllBooks() const {
    return fetchBooks("SELECT id, title, author, year FROM books ORDER BY id");
}

std::optional<Book> BookRepository::findById(int id) const {
    std::ostringstream sql;
    sql << "SELECT id, title, author, year FROM books WHERE id = " << id
        << " LIMIT 1";
    auto books = fetchBooks(sql.str());
    if (books.empty()) return std::nullopt;
    return books[0];
}

bool BookRepository::updateBook(const Book& book) {
    std::ostringstream sql;
    sql << "UPDATE books SET "
        << "title = '"  << db_.escapeString(book.title)  << "', "
        << "author = '" << db_.escapeString(book.author) << "', "
        << "year = "    << book.year
        << " WHERE id = " << book.id;
    return db_.execute(sql.str());
}

bool BookRepository::deleteBook(int id) {
    std::ostringstream sql;
    sql << "DELETE FROM books WHERE id = " << id;
    return db_.execute(sql.str());
}

// ── Utility queries ───────────────────────────────────────────────────────────

bool BookRepository::idExists(int id) const {
    return findById(id).has_value();
}

int BookRepository::count() const {
    MYSQL_RES* res = db_.query("SELECT COUNT(*) FROM books");
    if (!res) return 0;
    MYSQL_ROW row = mysql_fetch_row(res);
    int cnt = (row && row[0]) ? std::stoi(row[0]) : 0;
    mysql_free_result(res);
    return cnt;
}

// ── Sorted views ──────────────────────────────────────────────────────────────

std::vector<Book> BookRepository::sortByTitle() const {
    return fetchBooks(
        "SELECT id, title, author, year FROM books ORDER BY title ASC");
}

std::vector<Book> BookRepository::sortByYear() const {
    return fetchBooks(
        "SELECT id, title, author, year FROM books ORDER BY year ASC, title ASC");
}

std::vector<Book> BookRepository::sortByAuthor() const {
    return fetchBooks(
        "SELECT id, title, author, year FROM books ORDER BY author ASC, title ASC");
}
