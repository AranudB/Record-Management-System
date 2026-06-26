/**
 * @file Database.h
 * @brief MySQL connection RAII wrapper.
 *
 * Owns one MYSQL* handle; connects on demand and frees all resources
 * in the destructor (RAII).  Non-copyable to prevent double-free.
 */
#pragma once

#include <mysql/mysql.h>
#include <string>
#include <stdexcept>
#include <iostream>

class Database {
public:
    Database(const std::string& host,
             const std::string& user,
             const std::string& password,
             const std::string& dbName,
             int port = 3306);
    ~Database();

    // Non-copyable
    Database(const Database&)            = delete;
    Database& operator=(const Database&) = delete;

    /** Open the connection.  Returns false and prints error on failure. */
    bool  connect();

    /** Close the connection (also called by destructor). */
    void  disconnect();

    bool  isConnected() const { return conn_ != nullptr; }

    /**
     * Switch to a different database on the already-open connection.
     * Equivalent to running USE <dbName> but uses the C API directly.
     */
    bool selectDatabase(const std::string& dbName);

    /** Run a statement that returns no result set (INSERT / UPDATE / DELETE). */
    bool       execute(const std::string& sql);

    /** Run a SELECT; caller must call mysql_free_result() on the result. */
    MYSQL_RES* query(const std::string& sql);

    /**
     * Escape a string for safe use inside SQL string literals.
     * Uses mysql_real_escape_string so the current charset is respected.
     */
    std::string escapeString(const std::string& input) const;

private:
    MYSQL*      conn_;
    std::string host_;
    std::string user_;
    std::string password_;
    std::string dbName_;
    int         port_;
};