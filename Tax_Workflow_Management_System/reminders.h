#pragma once

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include "CSV_management.h"

using namespace std;

namespace TaxReturnSystem {

    // Class representing a single reminder
    class Reminder {
    private:
        // Reminder data members
        int projectId; // ID of the associated project
        chrono::system_clock::time_point dueDate; // Due date for the reminder
        string message; // Message associated with the reminder
        string userName; // User's name for personalized emails

    public:
        Reminder(int projectId, const chrono::system_clock::time_point& dueDate, const string& message, const string& userName); // Constructor

        // Getter methods
        int getProjectId() const; // Get project ID
        chrono::system_clock::time_point getDueDate() const; // Get reminder due date
        string getMessage() const; // Get reminder message
        string getUserName() const; // Get associated username
    };

    // Class for managing the reminder system
    class ReminderSystem {
    private:
        // System data members
        vector<Project> projects; // List of projects
        vector<Reminder> reminders; // List of reminders
        mutable ofstream logFile; // Logging file stream

        // CSV handling methods
        bool validateCSVRow(const vector<string>& row); // Validate CSV row data
        vector<string> splitCSVLine(const string& line); // Split CSV line into components
        chrono::system_clock::time_point parseDate(const string& dateStr); // Parse date string to time_point

        void log(const string& message) const; // Log system events

    public:
        ReminderSystem(); // Constructor
        ~ReminderSystem(); // Destructor

        // Reminder management methods
        void addReminder(int projectId, const chrono::system_clock::time_point& dueDate, const string& message, const string& userName); // Add new reminder
        void removeReminder(int projectId); // Remove reminder by project ID
        vector<Reminder> getRemindersForProject(int projectId) const; // Get reminders for specific project
        vector<Reminder> getOverdueReminders() const; // Get all overdue reminders

        // Utility methods
        string formatDate(const chrono::system_clock::time_point& date); // Format date for display
        void sendEmailReminder(const string& userRole, const string& userName); // Send email reminder
        void checkAndTriggerReminders(const string& userRole, const string& userName); // Check and trigger due reminders
        void loadRemindersFromCSV(const string& filename); // Load reminders from CSV file
    };

} // namespace TaxReturnSystem