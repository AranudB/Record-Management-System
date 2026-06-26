#include "InputValidator.h"
#include <iostream>
#include <cctype>

// ── Stream helpers ────────────────────────────────────────────────────────────

void InputValidator::clearInputStream() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// ── Public API ────────────────────────────────────────────────────────────────

std::string InputValidator::readString(const std::string& prompt, int maxLen) {
    while (true) {
        std::cout << prompt;
        std::string value;
        std::getline(std::cin, value);

        // Trim leading and trailing whitespace
        const auto first = value.find_first_not_of(" \t\r\n");
        const auto last  = value.find_last_not_of(" \t\r\n");

        if (first == std::string::npos) {
            std::cout << "  [!] Field cannot be empty. Please try again.\n";
            continue;
        }
        value = value.substr(first, last - first + 1);

        if (static_cast<int>(value.size()) > maxLen) {
            std::cout << "  [!] Input too long (max " << maxLen << " characters).\n";
            continue;
        }
        return value;
    }
}

int InputValidator::readInt(const std::string& prompt, int min, int max) {
    while (true) {
        std::cout << prompt;
        int value;
        if (std::cin >> value) {
            // Make sure nothing illegal follows the number on the same line
            char next = static_cast<char>(std::cin.peek());
            if (next == '\n' || next == '\r' || next == EOF) {
                clearInputStream();
                if (value >= min && value <= max) return value;
                std::cout << "  [!] Please enter a value between "
                          << min << " and " << max << ".\n";
            } else {
                clearInputStream();
                std::cout << "  [!] Invalid input. Please enter a whole number.\n";
            }
        } else {
            clearInputStream();
            std::cout << "  [!] Invalid input. Please enter a whole number.\n";
        }
    }
}

int InputValidator::readYear(const std::string& prompt) {
    return readInt(prompt, 1450, 2100);
}

int InputValidator::readId(const std::string& prompt) {
    return readInt(prompt, 1, std::numeric_limits<int>::max());
}

bool InputValidator::confirm(const std::string& prompt) {
    while (true) {
        std::cout << prompt << " [y/n]: ";
        char c;
        if (std::cin >> c) {
            clearInputStream();
            const char lc = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if (lc == 'y') return true;
            if (lc == 'n') return false;
        } else {
            clearInputStream();
        }
        std::cout << "  [!] Please enter 'y' or 'n'.\n";
    }
}
