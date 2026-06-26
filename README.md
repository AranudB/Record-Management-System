# Record Management System

A file-based CLI application to manage library book records (C++ + MySQL), written entirely in OOP.

---

## Features

| Feature | Details |
|---|---|
| **Add** | Insert a new book (title, author, year) |
| **Display all** | List every record in a formatted table |
| **Search by ID** | Look up a specific record; graceful "not found" message |
| **Update** | Edit any field in-place; press Enter to keep current value |
| **Delete** | Remove a record with confirmation prompt |
| **Sort** | Order records by title, year, or author (SQL `ORDER BY`) |
| **Export CSV** | Write all records to a RFC-4180-compliant `.csv` file |
| **Input validation** | All inputs validated in a loop; no raw string→SQL |
| **SQL injection safety** | `mysql_real_escape_string()` on all user strings |
| **UTF-8** | Database and connection both use `utf8mb4` |

---

## Requirements

| Tool | Version | Install (Ubuntu/Debian) |
|---|---|---|
| g++ | ≥ 9 (C++17) | `sudo apt install build-essential` |
| MySQL server | ≥ 5.7 | `sudo apt install mysql-server` |
| MySQL client dev headers | any | `sudo apt install libmysqlclient-dev` |
| pkg-config | any | `sudo apt install pkg-config` |

---

## Quick Start

### 1 — Build

**With Make (recommended)**
```bash
make
```

**With CMake**
```bash
cmake -B build
cmake --build build
```

### 2 — Run

```bash
./rms          # Make
./build/rms    # CMake
```

On the **first launch**, the program will ask for your MySQL credentials interactively:

```
  First-run setup — MySQL login
  MySQL host [localhost]:
  MySQL port [3306]:
  MySQL user: libraryuser
  MySQL password:

  [OK] Setup complete. Credentials saved.
  [OK] Connected to 'library_db' on localhost:3306
```

The database and table are created automatically. Credentials are saved to
`~/.library_rms/config.ini` — no manual SQL setup needed.

To re-run setup (e.g. to change credentials), simply delete the config file:
```bash
rm ~/.library_rms/config.ini
```

> **Tip (Ubuntu/Debian):** if your MySQL root uses socket authentication,
> create a dedicated user first:
> ```bash
> sudo mysql
> ```
> ```sql
> CREATE USER 'libraryuser'@'localhost' IDENTIFIED BY 'yourpassword';
> GRANT ALL PRIVILEGES ON library_db.* TO 'libraryuser'@'localhost';
> FLUSH PRIVILEGES;
> ```

---

## Project Structure

```
record_management_system/
├── src/
│   ├── main.cpp            ← entry point + DB credentials
│   ├── Book.h / .cpp       ← Book data model
│   ├── Database.h / .cpp   ← MySQL RAII connection wrapper
│   ├── BookRepository.h / .cpp  ← all SQL / CRUD logic
│   ├── InputValidator.h / .cpp  ← safe validated CLI input
│   ├── CSVExporter.h / .cpp     ← RFC-4180 CSV writer
│   └── Menu.h / .cpp       ← CLI interaction loop
├── sql/
│   └── schema.sql          ← CREATE DATABASE + CREATE TABLE + seed data
├── Makefile
├── CMakeLists.txt
└── README.md
```

---

## Architecture

```
            +--------+
            |  Menu  |   CLI interaction loop
            +---+----+
                | uses
     +----------+----------+
     |          |          |
     v          v          v
  BookRepo  InputVal  CSVExporter
     |
     v
  Database  (RAII MYSQL* wrapper)
     |
  MySQL Server
```

### Class Overview

| Class | File | Responsibility |
|---|---|---|
| `Book` | `Book.h/cpp` | Data model: id, title, author, year + display helpers |
| `Database` | `Database.h/cpp` | Own/open/close MYSQL*; execute queries; escape strings |
| `BookRepository` | `BookRepository.h/cpp` | All SQL: add, get, find, update, delete, sort |
| `InputValidator` | `InputValidator.h/cpp` | Validated CLI reads; never leaves cin in a bad state |
| `CSVExporter` | `CSVExporter.h/cpp` | Write RFC-4180 CSV with proper field escaping |
| `Menu` | `Menu.h/cpp` | Interaction loop; delegates to all other classes |

---

## Database Schema

```sql
CREATE TABLE books (
    id         INT          NOT NULL AUTO_INCREMENT,
    title      VARCHAR(255) NOT NULL,
    author     VARCHAR(255) NOT NULL,
    year       INT          NOT NULL,
    created_at TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE …,
    PRIMARY KEY (id)
);
```

Indexes on `title`, `author`, and `year` speed up the sort operations.