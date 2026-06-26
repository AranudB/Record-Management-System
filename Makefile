# ── Record Management System – Makefile ─────────────────────────────────────
#
# Requires:
#   g++ (C++17)  |  libmysqlclient-dev  |  pkg-config
#
# Ubuntu/Debian:  sudo apt install build-essential libmysqlclient-dev
# macOS (brew):   brew install mysql-client  &&  export PKG_CONFIG_PATH=...
#
# Build:   make
# Run:     ./rms
# Clean:   make clean

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic $(shell mysql_config --cflags)
LDFLAGS  := $(shell mysql_config --libs)

TARGET   := rms
SRC_DIR  := src

SRCS := $(SRC_DIR)/main.cpp          \
        $(SRC_DIR)/Book.cpp          \
        $(SRC_DIR)/Database.cpp      \
        $(SRC_DIR)/BookRepository.cpp\
        $(SRC_DIR)/InputValidator.cpp\
        $(SRC_DIR)/CSVExporter.cpp   \
        $(SRC_DIR)/Menu.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "\n  Build successful → ./$@\n"

clean:
	rm -f $(TARGET)
