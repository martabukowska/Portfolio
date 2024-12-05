#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <optional>
#include <unordered_map>
#include <sqlite3.h>
#include <mutex>
#include <iomanip>
#include <sstream>
#include <set>
#include <map>
#include "config.h"
#include "CSV_management.h"

using namespace std;

namespace TaxReturnSystem {

    // Represents a date stored in YYYYMMDD format
    class Date {
    private:
        int dateValue; // Stores the date as an integer

    public:
        Date() : dateValue(0) {} // Default constructor
        explicit Date(const string& dateStr) { setDate(dateStr); } // Constructor with date string

        void setDate(const string& dateStr); // Set the date from a string in the format YYYY-MM-DD
        string getDateStr() const; // Return the date as a string in the format YYYY-MM-DD

        // Comparison operators for Date
        bool operator<=(const Date& rhs) const { return dateValue <= rhs.dateValue; }
        bool operator>=(const Date& rhs) const { return dateValue >= rhs.dateValue; }
        bool operator!=(const Date& rhs) const { return dateValue != rhs.dateValue; }
        bool operator<(const Date& rhs) const { return dateValue < rhs.dateValue; }
        bool operator>(const Date& rhs) const { return dateValue > rhs.dateValue; }
        bool operator==(const Date& rhs) const { return dateValue == rhs.dateValue; }
    };

    // Enum for report types
    enum class ReportType {
        RegularDeadline,
        InternalDeadline
    };

    // Enum for dependency types
    enum class DependencyType {
        BEFORE,
        AFTER
    };

    // Represents a project with various attributes
    class Project {
    private:
        string id; // Unique identifier for the project
        static int nextId; // Static variable for generating unique IDs
        static mutex idMutex; // Mutex for thread-safe ID generation
        string group, client, projectType, billingPartner, partner, manager, nextTask, memo;
        Date regularDeadline, internalDeadline;
        bool extended;
        unordered_map<string, DependencyType> dependencies;
        ReportType reportType;

    public:
        Project() : id(generateId()), extended(false) {} // Default constructor

        string generateId() const { // Generate a unique ID for the project
            string generatedId = group + "|" + client + "|" + projectType;
            return generatedId;
        }

        // Getters and setters for project attributes
        string getId() const { return id; }
        void setId(const string& newId) { id = newId; }

        void saveToAppropriateVariable(int columnNum, const string& cellVal, const ReportType type); // Assign a value to the appropriate member variable based on the column number

        // Getters
        string getGroup() const { return group; }
        string getClient() const { return client; }
        string getProjectType() const { return projectType; }
        string getBillingPartner() const { return billingPartner; }
        string getPartner() const { return partner; }
        string getManager() const { return manager; }
        string getNextTask() const { return nextTask; }
        string getMemo() const { return memo; }
        Date getRegularDeadline() const { return regularDeadline; }
        Date getInternalDeadline() const { return internalDeadline; }
        unordered_map<string, DependencyType> getDependencies() const { return dependencies; }
        ReportType getReportType() const { return reportType; }

        // Setters
        void setGroup(const string& newGroup) { group = newGroup; }
        void setClient(const string& newClient) { client = newClient; }
        void setProjectType(const string& newProjectType) { projectType = newProjectType; }
        void setBillingPartner(const string& newBillingPartner) { billingPartner = newBillingPartner; }
        void setPartner(const string& newPartner) { partner = newPartner; }
        void setManager(const string& newManager) { manager = newManager; }
        void setNextTask(const string& newNextTask) { nextTask = newNextTask; }
        void setMemo(const string& newMemo) { memo = newMemo; }
        void setRegularDeadline(const Date& newRegularDeadline) { regularDeadline = newRegularDeadline; }
        void setInternalDeadline(const Date& newInternalDeadline) { internalDeadline = newInternalDeadline; }
        void setReportType(ReportType newReportType) { reportType = newReportType; }

        // Dependency management methods
        void addDependency(const string& projectId, DependencyType type) { dependencies[projectId] = type; }
        void removeDependency(string projectId) { dependencies.erase(projectId); }
        bool hasDependency(string projectId) const { return dependencies.find(projectId) != dependencies.end(); }
        void clearDependencies() { dependencies.clear(); }
        bool isDependencyMet(const string& dependencyId, const string& dependencyStatus) const;
        void buildPTETDependency(const vector<Project>& allProjects);

        bool isInDeadline(const Date& deadline, ReportType deadlineType) const;

        bool isExtended() const { return extended; }
        void setExtended(bool isExtended) { extended = isExtended; }
    };

    // Manages the database of projects
    class ProjectsDatabase {
    private:
        string dbPath; // Path to the database file
        sqlite3* db; // SQLite database connection

        void executeQuery(const string& query); // Execute a SQL query

        // Methods for database connection management
        bool openDatabase();
        void closeDatabase();
        bool initializeDatabase();
        bool isInTransaction = false; // Transaction state

    public:
        ProjectsDatabase(const string& dbPath); // Constructor
        ~ProjectsDatabase(); // Destructor

        void createTablesIfNotExist(); // Helper function for constructor

        // CRUD operations for projects
        bool addProjectToDatabase(const Project& project);
        bool getProjectFromDatabase(const string& id, Project& project);
        bool updateProjectInDatabase(const Project& project);
        bool deleteProjectFromDatabase(const string& id);

        // Methods to retrieve projects based on various criteria
        vector<Project> getAllProjects() const;
        vector<Project> searchProjects(const string& searchTerm) const;
        vector<Project> getProjectsByDateRange(const Date& startDate, const Date& endDate, ReportType reportType) const;
        vector<Project> getProjectsByManager(const string& manager) const;
        vector<Project> getProjectsByPartner(const string& partner) const;
        vector<Project> getProjectsByBillingPartner(const string& billingPartner) const;

        string getDbPath() const { return dbPath; } // Get database path

        void listAllProjects() const; // List all projects (for debugging)

        bool testDatabaseWrite();
        bool commitTransaction();
        bool beginTransaction();
        bool rollbackTransaction();

        static void updateColumnMappingsFromCSVHeader(const string& headerLine);
    };

    // Struct for filter criteria
    struct FilterCriteria {
        optional<string> group, projectType, billingPartner, partner, manager, nextTask;
        optional<Date> startDate, endDate;
        optional<Date> regularDeadline, internalDeadline;
        optional<bool> extended;
        optional<ReportType> reportType;
    };

    // Manages project operations and filtering
    class ProjectManager {
    private:
        ProjectsDatabase database; // Database of projects
        FilterCriteria currentFilter; // Current filter criteria

    public:
        ProjectManager(const string& dbPath) : database(dbPath) {} // Constructor

        static string getBillingPartner(const string& cellVal); // Extract billing partner from a cell value

        // CSV import/export operations
        bool importFromCSV(const string& filename, ReportType reportType);
        void exportToCSV(const string& filename, ReportType reportType) const;

        // Filter management
        void setFilter(FilterType filterType, const string& value);
        void setDateRangeFilter(const string& startDate, const string& endDate);
        void clearFilter(FilterType filterType);
        void resetFilter();
        vector<Project> getFilteredProjects() const;

        vector<Project> searchProjects(const string& searchTerm) const; // Search projects based on a search term

        void displayFilteredData(const vector<Project>& projects) const; // Display filtered data for testing purposes

        vector<Project> filterByDateRange(const Date& startDate, const Date& endDate, ReportType reportType) const; // Filter projects by date range

        vector<Project> getAllProjects(); // Get all projects

        static bool isProjectExtended(const string& cellVal); // Check if project is extended
    };

} // namespace TaxReturnSystem