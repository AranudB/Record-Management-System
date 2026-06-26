cmake_minimum_required(VERSION 3.15)
project(RecordManagementSystem CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Find MySQL ────────────────────────────────────────────────────────────────
find_package(PkgConfig REQUIRED)
pkg_check_modules(MYSQL REQUIRED mysqlclient)

# ── Sources ───────────────────────────────────────────────────────────────────
set(SOURCES
    src/main.cpp
    src/Book.cpp
    src/Database.cpp
    src/BookRepository.cpp
    src/InputValidator.cpp
    src/CSVExporter.cpp
    src/Menu.cpp
)

# ── Target ───────────────────────────────────────────────────────────────────
add_executable(rms ${SOURCES})

target_include_directories(rms PRIVATE src ${MYSQL_INCLUDE_DIRS})
target_link_libraries(rms PRIVATE ${MYSQL_LINK_LIBRARIES})
target_compile_options(rms PRIVATE -Wall -Wextra -pedantic)
