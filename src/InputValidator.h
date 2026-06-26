/**
 * @file InputValidator.h
 * @brief Safe, validated CLI input helpers.
 *
 * All methods loop until valid input is received, so the rest of the
 * application never has to deal with bad cin states.
 */
#pragma once

#include <string>
#include <limits>

class InputValidator {
public:
    /**
     * Read a non-empty, trimmed string, rejecting inputs longer than maxLen.
     * Uses std::getline, so spaces are preserved.
     */
    static std::string readString(const std::string& prompt, int maxLen = 255);

    /**
     * Read an integer in [min, max].  Rejects non-numeric and out-of-range.
     * Uses operator>> then clears the stream.
     */
    static int readInt(const std::string& prompt,
                       int min = std::numeric_limits<int>::min(),
                       int max = std::numeric_limits<int>::max());

    /** Convenience: readInt with year bounds 1450–2100. */
    static int readYear(const std::string& prompt);

    /** Convenience: readInt with lower bound 1 (valid DB IDs). */
    static int readId(const std::string& prompt);

    /** Ask a yes/no question.  Returns true for 'y'/'Y'. */
    static bool confirm(const std::string& prompt);

    /** Drain and reset the cin stream after a failed >> extraction. */
    static void clearInputStream();
};
