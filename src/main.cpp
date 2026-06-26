/**
 * @file main.cpp
 * @brief Entry point with fully automatic first-run setup.
 *
 * First launch flow:
 *   1. Ask for MySQL host / port / credentials.
 *   2. Try to connect.
 *   3. If "Access denied" (common on Ubuntu where root uses socket auth):
 *        offer to create a dedicated app user automatically via `sudo mysql`.
 *   4. Create the database and books table (CREATE … IF NOT EXISTS).
 *   5. Save working credentials to ~/.library_rms/config.ini.
 *
 * All subsequent launches: load config and start immediately.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <filesystem>

#include "Database.h"
#include "BookRepository.h"
#include "Menu.h"
#include "InputValidator.h"

// ── Paths ─────────────────────────────────────────────────────────────────────
static const std::string CONFIG_DIR  = []() -> std::string {
    const char* h = std::getenv("HOME");
    return std::string(h ? h : ".") + "/.library_rms";
}();
static const std::string CONFIG_FILE = CONFIG_DIR + "/config.ini";
static const std::string DB_NAME     = "library_db";

// ── Embedded schema ───────────────────────────────────────────────────────────
static const std::string SQL_CREATE_DB =
    "CREATE DATABASE IF NOT EXISTS library_db "
    "CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci";

static const std::string SQL_CREATE_TABLE =
    "CREATE TABLE IF NOT EXISTS books ("
    "  id         INT          NOT NULL AUTO_INCREMENT,"
    "  title      VARCHAR(255) NOT NULL,"
    "  author     VARCHAR(255) NOT NULL,"
    "  year       INT          NOT NULL,"
    "  created_at TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP,"
    "  updated_at TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP"
    "                                   ON UPDATE CURRENT_TIMESTAMP,"
    "  PRIMARY KEY (id),"
    "  INDEX idx_title  (title),"
    "  INDEX idx_author (author),"
    "  INDEX idx_year   (year)"
    ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

// ── Config ────────────────────────────────────────────────────────────────────
struct AppConfig {
    std::string host     = "localhost";
    std::string user     = "";
    std::string password = "";
    int         port     = 3306;

    bool fileExists() const { return std::filesystem::exists(CONFIG_FILE); }

    bool load() {
        std::ifstream f(CONFIG_FILE);
        if (!f) return false;
        std::string line;
        while (std::getline(f, line)) {
            auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            auto key = line.substr(0, eq);
            auto val = line.substr(eq + 1);
            if      (key == "host")     host     = val;
            else if (key == "user")     user     = val;
            else if (key == "password") password = val;
            else if (key == "port") try { port = std::stoi(val); } catch (...) {}
        }
        return !user.empty();
    }

    bool save() const {
        std::filesystem::create_directories(CONFIG_DIR);
        std::ofstream f(CONFIG_FILE);
        if (!f) return false;
        f << "host="     << host     << "\n"
          << "user="     << user     << "\n"
          << "password=" << password << "\n"
          << "port="     << port     << "\n";
        return true;
    }
};

// ── Create a MySQL user via `sudo mysql` (handles Ubuntu socket auth) ─────────
static bool createUserViaSudo(const std::string& newUser,
                              const std::string& newPass) {
    // Escape single quotes in password just in case
    auto esc = [](const std::string& s) {
        std::string r;
        for (char c : s) { if (c == '\'') r += "\\'"; else r += c; }
        return r;
    };

    // Write SQL to a temp file (avoids password appearing in process list)
    const std::string tmpFile = "/tmp/.library_rms_init.sql";
    {
        std::ofstream f(tmpFile);
        if (!f) { std::cerr << "  [!!] Cannot write to /tmp\n"; return false; }
        f << "CREATE USER IF NOT EXISTS '"
          << esc(newUser) << "'@'localhost' "
          << "IDENTIFIED BY '" << esc(newPass) << "';\n"
          << "GRANT ALL PRIVILEGES ON library_db.* TO '"
          << esc(newUser) << "'@'localhost';\n"
          << "FLUSH PRIVILEGES;\n";
    }

    std::cout << "\n  Running sudo mysql — you may be prompted for your system password:\n\n";
    const int ret = std::system(("sudo mysql < " + tmpFile).c_str());
    std::remove(tmpFile.c_str());   // delete immediately

    return (ret == 0);
}

// ── Bootstrap DB + table on an open connection ────────────────────────────────
static bool bootstrapSchema(Database& db) {
    if (!db.execute(SQL_CREATE_DB)) {
        std::cerr << "  [!!] Could not create database.\n";
        return false;
    }
    if (!db.selectDatabase(DB_NAME)) {
        std::cerr << "  [!!] Could not select database.\n";
        return false;
    }
    if (!db.execute(SQL_CREATE_TABLE)) {
        std::cerr << "  [!!] Could not create table.\n";
        return false;
    }
    return true;
}

// ── Try to connect; return false and set errMsg on failure ────────────────────
static bool tryConnect(Database& db, std::string& errMsg) {
    // Redirect stderr temporarily to capture mysql error text
    if (!db.connect()) {
        // mysql errors are already printed to stderr by Database::connect()
        errMsg = "connection failed";
        return false;
    }
    return true;
}

// ── First-run interactive setup ───────────────────────────────────────────────
static bool firstRunSetup(AppConfig& cfg) {
    std::cout << "\n"
              << "  ╔══════════════════════════════════════════╗\n"
              << "  ║       First-run setup — MySQL login      ║\n"
              << "  ╚══════════════════════════════════════════╝\n\n";

    // Host / port
    std::cout << "  MySQL host [localhost]: ";
    std::string in; std::getline(std::cin, in);
    if (!in.empty()) cfg.host = in;

    std::cout << "  MySQL port [3306]: ";
    std::getline(std::cin, in);
    if (!in.empty()) try { cfg.port = std::stoi(in); } catch (...) {}

    // Credentials
    cfg.user = InputValidator::readString("  MySQL user: ");
    std::cout << "  MySQL password (leave blank if none): ";
    std::getline(std::cin, cfg.password);

    // First connection attempt
    std::cout << "\n  Connecting...\n";
    {
        Database db(cfg.host, cfg.user, cfg.password, "", cfg.port);
        std::string err;
        if (tryConnect(db, err)) {
            // Success — bootstrap schema and save
            std::cout << "  [OK] Connected.\n";
            if (!bootstrapSchema(db)) return false;
            cfg.save();
            std::cout << "  [OK] Database ready. Credentials saved.\n";
            return true;
        }
    }

    // Connection failed — offer automatic user creation via sudo mysql
    std::cout << "\n"
              << "  Could not connect with those credentials.\n"
              << "  On Ubuntu/Debian, MySQL root often uses socket authentication\n"
              << "  (password login is disabled by default).\n\n"
              << "  The program can create a dedicated MySQL user automatically\n"
              << "  using 'sudo mysql' — this requires your system (sudo) password.\n\n";

    if (!InputValidator::confirm("  Create a dedicated MySQL user automatically?")) {
        std::cout << "\n  You can create one manually and relaunch the program:\n"
                  << "    sudo mysql\n"
                  << "    CREATE USER 'libraryuser'@'localhost' IDENTIFIED BY 'yourpassword';\n"
                  << "    GRANT ALL PRIVILEGES ON library_db.* TO 'libraryuser'@'localhost';\n"
                  << "    FLUSH PRIVILEGES;\n\n";
        return false;
    }

    // Ask for the new app-user credentials
    std::cout << "\n  Choose a username for the application user [libraryuser]: ";
    std::string newUser; std::getline(std::cin, newUser);
    if (newUser.empty()) newUser = "libraryuser";

    const std::string newPass = InputValidator::readString("  Choose a password for this user: ");

    // Run sudo mysql to create the user
    if (!createUserViaSudo(newUser, newPass)) {
        std::cerr << "\n  [!!] sudo mysql failed. "
                  << "Please create the user manually and relaunch.\n";
        return false;
    }
    std::cout << "  [OK] User '" << newUser << "' created.\n";

    // Update config with the new credentials and try again
    cfg.user     = newUser;
    cfg.password = newPass;

    std::cout << "  Connecting as '" << newUser << "'...\n";
    Database db2(cfg.host, cfg.user, cfg.password, "", cfg.port);
    std::string err2;
    if (!tryConnect(db2, err2)) {
        std::cerr << "  [!!] Still cannot connect. Please check MySQL and try again.\n";
        return false;
    }

    std::cout << "  [OK] Connected.\n";
    if (!bootstrapSchema(db2)) return false;
    cfg.save();
    std::cout << "  [OK] Database ready. Credentials saved to:\n"
              << "       " << CONFIG_FILE << "\n\n";
    return true;
}

// ── main ─────────────────────────────────────────────────────────────────────
int main() {
    AppConfig cfg;

    if (!cfg.fileExists()) {
        // ── First run ─────────────────────────────────────────────────────
        if (!firstRunSetup(cfg)) return EXIT_FAILURE;

    } else {
        // ── Subsequent runs ───────────────────────────────────────────────
        if (!cfg.load()) {
            std::cerr << "[Error] Cannot read " << CONFIG_FILE
                      << " — delete it to re-run setup.\n";
            return EXIT_FAILURE;
        }
    }

    // Open the working connection (DB already exists)
    Database db(cfg.host, cfg.user, cfg.password, DB_NAME, cfg.port);
    if (!db.connect()) {
        std::cerr << "[Error] Cannot connect. Delete " << CONFIG_FILE
                  << " to re-run setup.\n";
        return EXIT_FAILURE;
    }

    // Ensure schema is up-to-date on every launch (safe no-ops if already there)
    db.execute(SQL_CREATE_TABLE);

    std::cout << "[OK] Connected to '" << DB_NAME << "' on "
              << cfg.host << ":" << cfg.port << "\n";

    BookRepository repo(db);
    Menu           menu(repo);
    menu.run();

    return EXIT_SUCCESS;
}