/**
 * @file logger.cpp
 * @brief Implementation of logging functionality for the Tax Return System
 *
 * This file contains implementations for:
 * - File-based logging system
 * - Timestamp generation
 * - Event logging with automatic file handling
 *
 * The Logger class provides thread-safe logging capabilities
 * with automatic file management and timestamp formatting.
 */

#include "logger.h"
#include <ctime>

using namespace std;

namespace TaxReturnSystem {

// LOGGER CLASS METHODS:

    // Definition of constructor; takes filename as parameter
    Logger::Logger(const string &filename) {
        logFile.open(filename, ios::app); // Open file in append mode
    }

    // Definition of destructor; ensures file is closed if still open
    Logger::~Logger() {
        if (logFile.is_open()) {
            logFile.close(); // Close the file if it's open
        }
    }

    // Definition of method to log an event; takes event description as parameter
    void Logger::log(const string &event) {
        if (logFile.is_open()) {
            time_t now = time(0); // Get current time
            char timestamp[20];
            strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now)); // Format timestamp
            logFile << timestamp << " - " << event << endl; // Write timestamp and event to log file
        }
    }

}