/**
 * @file reminders.cpp
 * @brief Implementation of the Reminder and ReminderSystem classes for the Tax Return System
 *
 * This file contains implementations for:
 * - Managing reminders for projects
 * - Logging reminder system events
 * - Adding, removing, and retrieving reminders
 * - Loading reminders from CSV files
 * - Sending email reminders (placeholder)
 *
 * The ReminderSystem class provides comprehensive functionality for handling
 * project reminders, including logging, CSV integration, and basic email notification.
 */

#include "reminders.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <iomanip>

namespace TaxReturnSystem {

    // Reminder class implementation
    Reminder::Reminder(int projectId, const chrono::system_clock::time_point& dueDate, const string& message, const string& userName)
            : projectId(projectId), dueDate(dueDate), message(message), userName(userName) {}

    // Definition of a method to get project ID; takes no parameters; returns int
    int Reminder::getProjectId() const {
        return projectId;
    }

    // Definition of a method to get reminder due date; takes no parameters; returns system_clock time_point
    chrono::system_clock::time_point Reminder::getDueDate() const {
        return dueDate;
    }

    // Definition of a method to get reminder message; takes no parameters; returns string
    string Reminder::getMessage() const {
        return message;
    }

    // Definition of a method to get user name associated with the reminder; takes no parameters; returns string
    string Reminder::getUserName() const {
        return userName;
    }

    // Definition of a constructor to initialize reminder system and logging; takes no parameters; returns nothing
    ReminderSystem::ReminderSystem() {
        // Open log file in append mode
        logFile.open("reminder_system.log", ios::app);
        log("ReminderSystem initialized");
    }

    // Definition of a destructor to clean up reminder system resources; takes no parameters; returns nothing
    ReminderSystem::~ReminderSystem() {
        // Close log file if it's open
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    // Definition of a method to log system messages with timestamps; takes a string as parameter; returns void
    void ReminderSystem::log(const string& message) const {
        // Write timestamped message to log file if it's open
        if (logFile.is_open()) {
            time_t now = time(0);
            logFile << ctime(&now) << ": " << message << endl;
        }
    }

    // Definition of a method to format a time point as a string; takes a system_clock time_point as parameter; returns string
    string ReminderSystem::formatDate(const chrono::system_clock::time_point& date) {
        // Convert time_point to time_t
        time_t time = chrono::system_clock::to_time_t(date);

        // Format date and time using stringstream
        stringstream ss;
        ss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");

        return ss.str();
    }

    // Definition of a method to add a new reminder to the system; takes an int, a time_point, and two strings as parameters; returns void
    void ReminderSystem::addReminder(int projectId, const chrono::system_clock::time_point& dueDate, const string& message, const string& userName) {
        try {
            // Create and add new reminder to the collection
            reminders.emplace_back(projectId, dueDate, message, userName);

            // Log successful reminder creation
            log("Reminder added for project " + to_string(projectId) + " due " + formatDate(dueDate));
        } catch (const exception& e) {
            // Log error and rethrow exception
            log("Error adding reminder: " + string(e.what()));
            throw;
        }
    }

    // Definition of a method to remove all reminders for a project; takes an int as parameter; returns void
    void ReminderSystem::removeReminder(int projectId) {
        // Store initial count of reminders
        auto initialSize = reminders.size();

        // Remove all reminders matching the project ID
        reminders.erase(remove_if(reminders.begin(), reminders.end(),
                                  [projectId](const Reminder& r) { return r.getProjectId() == projectId; }),
                        reminders.end());

        // Log result based on whether any reminders were removed
        if (reminders.size() < initialSize) {
            log("Reminder(s) removed for project " + to_string(projectId));
        } else {
            log("No reminders found to remove for project " + to_string(projectId));
        }
    }

    // Definition of a method to get all reminders for a specific project; takes an int as parameter; returns vector of Reminders
    vector<Reminder> ReminderSystem::getRemindersForProject(int projectId) const {
        // Create vector to store matching reminders
        vector<Reminder> projectReminders;

        // Collect all reminders for the specified project
        for (const auto& reminder : reminders) {
            if (reminder.getProjectId() == projectId) {
                projectReminders.push_back(reminder);
            }
        }

        // Log retrieval result
        log("Retrieved " + to_string(projectReminders.size()) + " reminders for project " + to_string(projectId));

        return projectReminders;
    }

    // Definition of a method to get all overdue reminders; takes no parameters; returns vector of Reminders
    vector<Reminder> ReminderSystem::getOverdueReminders() const {
        // Create vector to store overdue reminders
        vector<Reminder> overdueReminders;

        // Get current time for comparison
        auto now = chrono::system_clock::now();

        // Collect all reminders that are past due
        for (const auto& reminder : reminders) {
            if (reminder.getDueDate() < now) {
                overdueReminders.push_back(reminder);
            }
        }

        // Log retrieval result
        log("Retrieved " + to_string(overdueReminders.size()) + " overdue reminders");

        return overdueReminders;
    }

    // Definition of a method to send email reminders to a user; takes two strings as parameters; returns void
    void ReminderSystem::sendEmailReminder(const string& userRole, const string& userName) {
        // Log start of email sending process
        log("Sending email reminders for user " + userName + " (Role: " + userRole + ")");

        // Process each reminder for email sending
        for (const auto& reminder : reminders) {
            // TODO: Implement actual email sending logic
            cout << "Sending email to " << reminder.getUserName() << " for project " << reminder.getProjectId() << endl;
        }

        // Log completion of email sending
        log("Email reminders sent");
    }

    // Definition of a method to validate CSV row data; takes a vector of strings as parameter; returns bool
    bool ReminderSystem::validateCSVRow(const vector<string>& row) {
        // Check for correct number of columns
        if (row.size() != 4) {
            log("Invalid number of columns in CSV row");
            return false;
        }

        // Verify project ID is not empty
        if (row[0].empty()) {
            log("Project ID cannot be empty");
            return false;
        }

        return true;
    }

    // Definition of a method to split CSV line into fields; takes a string as parameter; returns vector of strings
    vector<string> ReminderSystem::splitCSVLine(const string& line) {
        // Initialize vector to store CSV fields
        vector<string> row;

        // Create string stream from input line
        stringstream ss(line);
        string cell;

        // Split line by commas and collect fields
        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        return row;
    }

    // Definition of a method to parse date string into time_point; takes a string as parameter; returns system_clock time_point
    chrono::system_clock::time_point ReminderSystem::parseDate(const string& dateStr) {
        // Initialize time structure
        tm tm = {};

        // Parse date string using stringstream
        stringstream ss(dateStr);
        ss >> get_time(&tm, "%Y-%m-%d");

        // Check for parsing errors
        if (ss.fail()) {
            throw runtime_error("Failed to parse date: " + dateStr);
        }

        // Convert to system_clock time_point
        return chrono::system_clock::from_time_t(mktime(&tm));
    }

    // Definition of a method to load reminders from a CSV file; takes a string as parameter; returns void
    void ReminderSystem::loadRemindersFromCSV(const string& filename) {
        // Log start of CSV loading process
        log("Loading reminders from CSV file: " + filename);

        // Open CSV file for reading
        ifstream file(filename);
        if (!file.is_open()) {
            log("Error: Unable to open file " + filename);
            throw runtime_error("Unable to open file: " + filename);
        }

        string line;
        int lineNumber = 0;

        // Read and process each line in the CSV file
        while (getline(file, line)) {
            lineNumber++;
            try {
                // Split line into fields
                vector<string> row = splitCSVLine(line);

                // Validate and process CSV row
                if (validateCSVRow(row)) {
                    int projectId = stoi(row[0]);
                    addReminder(projectId, parseDate(row[1]), row[2], row[3]);
                    log("Processed line " + to_string(lineNumber) + ": Added reminder for project " + row[0]);
                } else {
                    log("Invalid CSV row at line " + to_string(lineNumber));
                }
            } catch (const exception& e) {
                // Log any errors encountered during processing
                log("Error processing CSV row at line " + to_string(lineNumber) + ": " + e.what());
            }
        }

        // Log completion of CSV loading process
        log("Finished loading reminders from CSV file: " + filename);
    }

    // Definition of a method to check and trigger reminders for a user; takes two strings as parameters; returns void
    void ReminderSystem::checkAndTriggerReminders(const string& userRole, const string& userName) {
        // TODO: Implement reminder checking and triggering logic
    }

}