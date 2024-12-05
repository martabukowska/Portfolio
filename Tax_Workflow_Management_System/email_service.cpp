/**
 * @file email_service.cpp
 * @brief Implementation of email service functionality for the Tax Return System
 *
 * This file contains implementations for:
 * - Email sending simulation
 * - Email logging
 * - Network delay simulation
 * - Success/failure rate simulation
 */

#include "email_service.h"
#include <thread>
#include <chrono>
#include <cstdlib>

namespace TaxReturnSystem {

    // Method to simulate sending an email; takes recipient email address, subject, and body as parameters; returns true if the email was "sent" successfully, false otherwise
    bool EmailService::sendEmail(const string& to, const string& subject, const string& body) {
        // Simulate network delay
        this_thread::sleep_for(chrono::milliseconds(500));
        // Log the email
        logEmail(to, subject, body);
        // Simulate occasional failure
        return (rand() % 100) < 95;  // 95% success rate
    }

    // Method to log email details; takes recipient email address, subject, and body as parameters
    void EmailService::logEmail(const string& to, const string& subject, const string& body) {
        ofstream logFile("email_log.txt", ios::app);
        if (logFile.is_open()) {
            time_t now = time(0);
            char* dt = ctime(&now);
            logFile << "Time: " << dt;
            logFile << "To: " << to << endl;
            logFile << "Subject: " << subject << endl;
            logFile << "Body: " << body << endl;
            logFile << "--------------------" << endl;
            logFile.close();
        }
    }

}