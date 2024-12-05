#pragma once

#include <fstream>
#include <string>

using namespace std;

namespace TaxReturnSystem {

    class Logger {
    private:
        ofstream logFile; // File stream for logging

    public:
        // Core logging methods
        Logger(const string &filename); // Initialize logger with file
        ~Logger(); // Clean up and close log file
        void log(const string &event); // Log event with timestamp
    };

} // namespace TaxReturnSystem