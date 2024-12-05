/**
 * @file main.cpp
 * @brief Main entry point for the Tax Return System
 *
 * This file initializes and runs the core application, including:
 * - Database connections (Users and Projects)
 * - Project management system
 * - Authentication service
 * - Email service
 * - Reminder system
 * - Web server (Crow) setup and routing
 * - CSV data import functionality
 *
 * The application runs on localhost:8080 and provides a web interface
 * for managing tax return projects and related services.
 */

#include "CSV_management.h"
#include "report_generator.h"
#include "statistics.h"
#include "reminders.h"
#include "routes.h"
#include <iostream>
#include <filesystem>
#include <memory>
#include <chrono>
#include "config.h"
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

using namespace std;

using namespace TaxReturnSystem;

// Global shared pointer to CSV data
shared_ptr<Project> globalCsvData;

// Global ReminderSystem object
ReminderSystem reminderSystem;

int main() {
    try {
        cout << "Starting the application..." << endl;
        cout << "Current working directory: " << filesystem::current_path() << endl;

        crow::SimpleApp app;

        cout << "Initializing dependencies..." << endl;
        EmailService emailService;

        cout << "Initializing UserDatabase..." << endl;
        UserDatabase userDatabase("./users.db");
        cout << "UserDatabase initialized successfully." << endl;

        cout << "Initializing ProjectsDatabase..." << endl;
        ProjectsDatabase projectsDatabase("./projects.db");
        cout << "ProjectsDatabase initialized successfully." << endl;

        cout << "Initializing ProjectManager..." << endl;
        ProjectManager projectManager("./projects.db");

        // Import CSV data
        cout << "Importing CSV data..." << endl;
        if (projectManager.importFromCSV("test-import.csv", ReportType::RegularDeadline)) {
            cout << "CSV data imported successfully." << endl;

            // Verify import by counting projects
            auto projects = projectManager.getAllProjects();
            cout << "Number of projects imported: " << projects.size() << endl;
        } else {
            cout << "Failed to import CSV data." << endl;
        }

        cout << "ProjectManager initialized successfully." << endl;

        Logger logger("auth.log");
        cout << "Dependencies initialized successfully." << endl;

        cout << "Initializing Auth object..." << endl;
        Auth auth(emailService, userDatabase, logger, "MC9QxA2/n2HSzum5urDW2CvrBDA7xylkKfQtUWEBils=");
        cout << "Auth object initialized successfully." << endl;

        cout << "Initializing global CSV Data..." << endl;
        globalCsvData = make_shared<Project>();
        cout << "Global CSV Data initialized successfully." << endl;

        cout << "Initializing ReminderSystem..." << endl;
        ReminderSystem reminderSystem;
        cout << "ReminderSystem initialized successfully." << endl;

        cout << "Setting up routes..." << endl;
        setupRoutes(app, auth, reminderSystem, projectManager);
        cout << "Routes set up successfully." << endl;

        // Run the app on localhost port 8080
        cout << "Starting the server on port 8080..." << endl;
        app.port(8080).multithreaded().run();
    }
    catch (const exception& e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1;
    }

    return 0;
}