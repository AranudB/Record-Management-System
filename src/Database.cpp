#include "Database.h"
#include <cstring>

Database::Database(const std::string& host,
                   const std::string& user,
                   const std::string& password,
                   const std::string& dbName,
                   int port)
    : conn_(nullptr), host_(host), user_(user),
      password_(password), dbName_(dbName), port_(port) {}

Database::~Database() {
    disconnect();
}

bool Database::connect() {
    conn_ = mysql_init(nullptr);
    if (!conn_) {
        std::cerr << "[DB] mysql_init() failed – out of memory?\n";
        return false;
    }

    if (!mysql_real_connect(conn_,
                            host_.c_str(),
                            user_.c_str(),
                            password_.c_str(),
                            dbName_.c_str(),
                            static_cast<unsigned int>(port_),
                            nullptr, 0)) {
        std::cerr << "[DB] Connection failed: " << mysql_error(conn_) << "\n";
        mysql_close(conn_);
        conn_ = nullptr;
        return false;
    }

    // Enforce UTF-8 for all text columns
    mysql_set_character_set(conn_, "utf8mb4");
    return true;
}

void Database::disconnect() {
    if (conn_) {
        mysql_close(conn_);
        conn_ = nullptr;
    }
}

bool Database::selectDatabase(const std::string& dbName) {
    if (!conn_) return false;
    if (mysql_select_db(conn_, dbName.c_str()) != 0) {
        std::cerr << "[DB] Failed to select database '" << dbName
                  << "': " << mysql_error(conn_) << "\n";
        return false;
    }
    dbName_ = dbName;
    return true;
}

bool Database::execute(const std::string& sql) {
    if (!conn_) {
        std::cerr << "[DB] execute() called but not connected.\n";
        return false;
    }
    if (mysql_query(conn_, sql.c_str()) != 0) {
        std::cerr << "[DB] Query error: " << mysql_error(conn_) << "\n"
                  << "     SQL: " << sql << "\n";
        return false;
    }
    return true;
}

MYSQL_RES* Database::query(const std::string& sql) {
    if (!execute(sql)) return nullptr;
    MYSQL_RES* res = mysql_store_result(conn_);
    if (!res && mysql_field_count(conn_) > 0) {
        std::cerr << "[DB] mysql_store_result() failed: " << mysql_error(conn_) << "\n";
    }
    return res;
}

std::string Database::escapeString(const std::string& input) const {
    if (!conn_) return input;
    // Worst case: every char is escaped + null terminator
    std::string escaped(input.size() * 2 + 1, '\0');
    unsigned long len = mysql_real_escape_string(
        conn_,
        &escaped[0],
        input.c_str(),
        static_cast<unsigned long>(input.size()));
    escaped.resize(len);
    return escaped;
}