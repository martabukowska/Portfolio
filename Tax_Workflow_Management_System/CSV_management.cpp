/**
 * @file CSV_management.cpp
 * @brief Implementation of CSV file management and project database operations
 *
 * This file contains implementations for:
 * - Date handling
 * - Project data management
 * - CSV import/export
 * - Database operations
 * - Project filtering
 */

#include "CSV_management.h"

using namespace std;

namespace TaxReturnSystem {

    mutex Project::idMutex;

// DATE CLASS METHODS:

    // Definition of a method to set the date from a string in format YYYY-MM-DD or MM/DD/YY; takes a string as parameter; returns void
    void Date::setDate(const string &dateStr) {

        if (dateStr.empty()) {
            dateValue = 0;
            return;
        }

        try {
            // Handle MM/DD/YY format
            if (dateStr.find('/') != string::npos) {
                vector<string> parts;
                stringstream ss(dateStr);
                string part;

                while (getline(ss, part, '/')) {
                    parts.push_back(part);
                }

                if (parts.size() == 3) {
                    int month = stoi(parts[0]);
                    int day = stoi(parts[1]);
                    int year = stoi(parts[2]);

                    // Convert 2-digit year to 4-digit year
                    if (year < 100) {
                        year += (year < 50) ? 2000 : 1900;
                    }

                    dateValue = year * 10000 + month * 100 + day;
                    return;
                }
            }

            // Handle YYYY-MM-DD format (original format)
            if (dateStr.length() == 10 && dateStr[4] == '-' && dateStr[7] == '-') {
                int year = stoi(dateStr.substr(0, 4));
                int month = stoi(dateStr.substr(5, 2));
                int day = stoi(dateStr.substr(8, 2));

                dateValue = year * 10000 + month * 100 + day;
                return;
            }

            dateValue = 0;

        } catch (const invalid_argument &e) {
            dateValue = 0;
        }
    }

    // Definition of a method to return the date as a string in format YYYY-MM-DD; takes no parameters; returns a string
    string Date::getDateStr() const {
        if (dateValue == 0) {
            return "";
        } else {
            int year = dateValue / 10000;
            int month = (dateValue / 100) % 100;
            int day = dateValue % 100;
            ostringstream oss;
            oss << setw(4) << setfill('0') << year << "-"
                << setw(2) << setfill('0') << month << "-"
                << setw(2) << setfill('0') << day;
            return oss.str();
        }
    }

// PROJECT CLASS METHODS:

    // Definition of a method to assign a cell value to the appropriate member variable based on column number; takes an int, a string, and a ReportType as parameters; returns void
    void Project::saveToAppropriateVariable(const int columnNum, const string &cellVal, const ReportType reportType) {
        const int MAX_COLUMNS = 11;

        if (columnNum >= MAX_COLUMNS) {
            cerr << "Warning: Ignoring extra column " << columnNum << endl;
            return;
        }

        string unquotedVal;
        if (cellVal.length() >= 2 && cellVal.front() == '"' && cellVal.back() == '"') {
            unquotedVal = cellVal.substr(1, cellVal.length() - 2);
        } else {
            unquotedVal = cellVal;
        }

        // Handle each column based on its index
        if (columnNum == GROUP_COLUMN) {
            group = unquotedVal;
        }
        else if (columnNum == CLIENT_COLUMN) {
            client = unquotedVal;
        }
        else if (columnNum == PROJECT_COLUMN) {
            projectType = unquotedVal;
        }
        else if (columnNum == DUE_DATE_COLUMN) {

            if (reportType == ReportType::RegularDeadline) {
                regularDeadline.setDate(unquotedVal);
                // If internal deadline isn't set, make it the same as regular deadline
                if (internalDeadline.getDateStr().empty()) {
                    internalDeadline = regularDeadline;
                }
            } else {
                internalDeadline.setDate(unquotedVal);
            }
        }
        else if (columnNum == BILLING_PARTNER_COLUMN) {
            string unquotedTags = unquotedVal;
            billingPartner = ProjectManager::getBillingPartner(unquotedTags);
            extended = ProjectManager::isProjectExtended(unquotedTags);
        }
        else if (columnNum == PARTNER_COLUMN || columnNum == 9) {
            if (!unquotedVal.empty()) {
                partner = unquotedVal;
            } else if (!billingPartner.empty()) {
                partner = billingPartner;
            }
        }
        else if (columnNum == MANAGER_COLUMN) {
            manager = unquotedVal;
        }
        else if (columnNum == NEXT_TASK_COLUMN) {
            nextTask = unquotedVal;
        }
        else if (columnNum == MEMO_COLUMN) {
            memo = unquotedVal;
        }
    }

    // Definition of a method to check if a project's dependency is met; takes two strings as parameters; returns bool
    bool Project::isDependencyMet(const string& dependencyId, const string& dependencyStatus) const {
        auto it = dependencies.find(dependencyId);
        if (it != dependencies.end()) {
            return dependencyStatus == "Billed";
        }
        return true;
    }

    // Definition of a method to build PTET dependency for a project; takes a vector of Projects as parameter; returns void
    void Project :: buildPTETDependency(const vector<Project>& allProjects) {
        if (projectType.find("PTET") != string::npos) {
            string year = projectType.substr(0, 4);
            string formProjectName = year + " Form";
            for (const auto& otherProject : allProjects) {
                if (otherProject.getProjectType() == formProjectName && otherProject.getClient() == this->client) {
                    this->addDependency(otherProject.getId(), DependencyType::BEFORE);
                    break;
                }
            }
        }
    }

    // Definition of a method to check if a project is in deadline; takes a Date and ReportType as parameters; returns bool
    bool Project :: isInDeadline(const Date& deadline, ReportType deadlineType) const {
        if (deadlineType == ReportType::RegularDeadline) {
            return regularDeadline == deadline;
        } else if (deadlineType == ReportType::InternalDeadline) {
            return internalDeadline == deadline;
        }
        return false;
    }


// PROJECTS DATABASE CLASS METHODS:

    // Definition of a constructor to open database and create tables; takes a string as parameter; returns nothing
    ProjectsDatabase::ProjectsDatabase(const string& dbPath) : dbPath(dbPath), db(nullptr) {
        if (!openDatabase()) {
            throw runtime_error("Failed to open database");
        }
        createTablesIfNotExist();
    }

    // Definition of a method to execute a SQL query; takes a string as parameter; returns void
    void ProjectsDatabase::executeQuery(const string& query) {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            string error = "SQL error: ";
            error += errMsg;
            sqlite3_free(errMsg);
            throw runtime_error(error);
        }
    }

    // Definition of a destructor to close database connection; takes no parameters; returns nothing
    ProjectsDatabase::~ProjectsDatabase() {
        closeDatabase();
    }

    // Definition of a method to open the database connection; takes no parameters; returns bool
    bool ProjectsDatabase::openDatabase() {
        int result = sqlite3_open(dbPath.c_str(), &db);
        if (result != SQLITE_OK) {
            cerr << "Failed to open database: " << sqlite3_errmsg(db) << endl;
            return false;
        }
        return true;
    }

    // Definition of a method to close the database connection; takes no parameters; returns void
    void ProjectsDatabase::closeDatabase() {
        if (db) {
            sqlite3_close(db);
            db = nullptr;
        }
    }

    // Definition of a method to retrieve all projects from database; takes no parameters; returns vector of Projects
    vector<Project> ProjectsDatabase::getAllProjects() const {
        vector<Project> projects;

        if (!db) {
            cerr << "Database connection is not open!" << endl;
            return projects;
        }

        string query = "SELECT id, project_group, client, project_type, billing_partner, partner, manager, "
                       "next_task, memo, regular_deadline, internal_deadline, extended, report_type "
                       "FROM projects;";

        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return projects;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Project project;

            // Get all the text fields safely
            const char* id = (const char*)sqlite3_column_text(stmt, 0);
            const char* group = (const char*)sqlite3_column_text(stmt, 1);
            const char* client = (const char*)sqlite3_column_text(stmt, 2);
            const char* projectType = (const char*)sqlite3_column_text(stmt, 3);
            const char* billingPartner = (const char*)sqlite3_column_text(stmt, 4);
            const char* partner = (const char*)sqlite3_column_text(stmt, 5);
            const char* manager = (const char*)sqlite3_column_text(stmt, 6);
            const char* nextTask = (const char*)sqlite3_column_text(stmt, 7);
            const char* memo = (const char*)sqlite3_column_text(stmt, 8);
            const char* regularDeadline = (const char*)sqlite3_column_text(stmt, 9);
            const char* internalDeadline = (const char*)sqlite3_column_text(stmt, 10);

            project.setId(id ? id : "");
            project.setGroup(group ? group : "");
            project.setClient(client ? client : "");
            project.setProjectType(projectType ? projectType : "");
            project.setBillingPartner(billingPartner ? billingPartner : "");
            project.setPartner(partner ? partner : "");
            project.setManager(manager ? manager : "");
            project.setNextTask(nextTask ? nextTask : "");
            project.setMemo(memo ? memo : "");

            if (regularDeadline) {
                project.setRegularDeadline(Date(regularDeadline));
            }
            if (internalDeadline) {
                project.setInternalDeadline(Date(internalDeadline));
            }

            project.setExtended(sqlite3_column_int(stmt, 11) != 0);
            project.setReportType(static_cast<ReportType>(sqlite3_column_int(stmt, 12)));

            projects.push_back(project);
        }

        sqlite3_finalize(stmt);
        return projects;
    }

    // Definition of a method to search projects in database; takes a string as parameter; returns vector of Projects
    vector<Project> ProjectsDatabase::searchProjects(const string& searchTerm) const {
        vector<Project> results;
        string query = "SELECT * FROM projects WHERE "
                       "id LIKE ? OR "
                       "project_group LIKE ? OR "
                       "client LIKE ? OR "
                       "project_type LIKE ? OR "
                       "billing_partner LIKE ? OR "
                       "partner LIKE ? OR "
                       "manager LIKE ? OR "
                       "next_task LIKE ? OR "
                       "memo LIKE ?";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return results;
        }

        string likePattern = "%" + searchTerm + "%";
        for (int i = 1; i <= 9; ++i) {
            sqlite3_bind_text(stmt, i, likePattern.c_str(), -1, SQLITE_STATIC);
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Project project;
            project.setId(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            project.setGroup(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            project.setClient(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            project.setProjectType(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            project.setBillingPartner(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            project.setPartner(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
            project.setManager(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
            project.setNextTask(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
            project.setMemo(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
            project.setRegularDeadline(Date(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9))));
            project.setInternalDeadline(Date(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10))));
            project.setExtended(sqlite3_column_int(stmt, 11) != 0);
            project.setReportType(static_cast<ReportType>(sqlite3_column_int(stmt, 12)));

            results.push_back(project);
        }

        sqlite3_finalize(stmt);
        return results;
    }

    // Definition of a method to create database tables if they don't exist; takes no parameters; returns void
    void ProjectsDatabase::createTablesIfNotExist() {
        const char* sql = R"(
        CREATE TABLE IF NOT EXISTS projects (
            id TEXT PRIMARY KEY ON CONFLICT REPLACE,
            project_group TEXT,
            client TEXT,
            project_type TEXT,
            billing_partner TEXT,
            partner TEXT,
            manager TEXT,
            next_task TEXT,
            memo TEXT,
            regular_deadline TEXT,
            internal_deadline TEXT,
            extended INTEGER DEFAULT 0,
            report_type INTEGER
        );
    )";

        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            string errorMsg = errMsg ? errMsg : "Unknown error";
            sqlite3_free(errMsg);
            throw runtime_error("Failed to create projects table: " + errorMsg);
        }
    }

    // Definition of a method to add a project to database; takes a Project as parameter; returns bool
    bool ProjectsDatabase::addProjectToDatabase(const Project& project) {

        string id = project.generateId();

        // Begin transaction
        executeQuery("BEGIN TRANSACTION;");

        string checkQuery = "SELECT * FROM projects WHERE id = ?;";
        sqlite3_stmt* checkStmt;
        bool projectExists = false;

        if (sqlite3_prepare_v2(db, checkQuery.c_str(), -1, &checkStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(checkStmt, 1, id.c_str(), -1, SQLITE_STATIC);

            if (sqlite3_step(checkStmt) == SQLITE_ROW) {
                projectExists = true;
            } else {
            }
            sqlite3_finalize(checkStmt);
        }

        // First delete the existing record if it exists
        if (projectExists) {
            string deleteQuery = "DELETE FROM projects WHERE id = ?;";
            sqlite3_stmt* deleteStmt;
            if (sqlite3_prepare_v2(db, deleteQuery.c_str(), -1, &deleteStmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(deleteStmt, 1, id.c_str(), -1, SQLITE_STATIC);
                if (sqlite3_step(deleteStmt) != SQLITE_DONE) {
                    cerr << "Failed to delete existing record: " << sqlite3_errmsg(db) << endl;
                }
                sqlite3_finalize(deleteStmt);
            }
        }

        // Now insert the new/updated record
        string query = "INSERT INTO projects (id, project_group, client, project_type, billing_partner, "
                       "partner, manager, next_task, memo, regular_deadline, internal_deadline, "
                       "extended, report_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            executeQuery("ROLLBACK;");
            return false;
        }

        auto bindLongText = [this, &stmt](int index, const string& text) {
            if (sqlite3_bind_text(stmt, index, text.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
                cerr << "Error binding text for column " << index << ": " << sqlite3_errmsg(db) << endl;
                return false;
            }
            return true;
        };

        // Bind all fields
        bool bindingSuccess = true;
        bindingSuccess &= bindLongText(1, id);
        bindingSuccess &= bindLongText(2, project.getGroup());
        bindingSuccess &= bindLongText(3, project.getClient());
        bindingSuccess &= bindLongText(4, project.getProjectType());
        bindingSuccess &= bindLongText(5, project.getBillingPartner());
        bindingSuccess &= bindLongText(6, project.getPartner());
        bindingSuccess &= bindLongText(7, project.getManager());
        bindingSuccess &= bindLongText(8, project.getNextTask());
        bindingSuccess &= bindLongText(9, project.getMemo());
        bindingSuccess &= bindLongText(10, project.getRegularDeadline().getDateStr());
        bindingSuccess &= bindLongText(11, project.getInternalDeadline().getDateStr());

        if (!bindingSuccess) {
            cerr << "Failed to bind one or more text fields\n";
            sqlite3_finalize(stmt);
            executeQuery("ROLLBACK;");
            return false;
        }

        if (sqlite3_bind_int(stmt, 12, project.isExtended() ? 1 : 0) != SQLITE_OK ||
            sqlite3_bind_int(stmt, 13, static_cast<int>(project.getReportType())) != SQLITE_OK) {
            cerr << "Error binding integer fields: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            executeQuery("ROLLBACK;");
            return false;
        }

        int result = sqlite3_step(stmt);
        if (result != SQLITE_DONE) {
            cerr << "Failed to add/update project in database: " << sqlite3_errmsg(db) << endl;
            cerr << "SQLite result code: " << result << endl;
            sqlite3_finalize(stmt);
            executeQuery("ROLLBACK;");
            return false;
        }

        sqlite3_finalize(stmt);
        executeQuery("COMMIT;");

        return true;
    }

    // Definition of a method to update a project in database; takes a Project as parameter; returns bool
    bool ProjectsDatabase::updateProjectInDatabase(const Project& project) {
        string query = "UPDATE projects SET billing_partner = ?, partner = ?, manager = ?, next_task = ?, memo = ?, regular_deadline = ?, internal_deadline = ?, extended = ?, report_type = ? WHERE id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, project.getBillingPartner().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, project.getPartner().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, project.getManager().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, project.getNextTask().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, project.getMemo().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, project.getRegularDeadline().getDateStr().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, project.getInternalDeadline().getDateStr().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 8, project.isExtended() ? 1 : 0);
        sqlite3_bind_int(stmt, 9, static_cast<int>(project.getReportType()));
        sqlite3_bind_text(stmt, 10, project.getId().c_str(), -1, SQLITE_STATIC);

        int result = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (result != SQLITE_DONE) {
            cerr << "Failed to update project: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        return true;
    }

    // Definition of a method to delete a project from database by ID; takes a string as parameter; returns bool
    bool ProjectsDatabase::deleteProjectFromDatabase(const string& id) {
        string query = "DELETE FROM projects WHERE id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);

        int result = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (result != SQLITE_DONE) {
            cerr << "Failed to delete project: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        return true;
    }

    // Definition of a method to get a project from database by ID; takes a string and Project reference as parameters; returns bool
    bool ProjectsDatabase::getProjectFromDatabase(const string& id, Project& project) {
        string query = "SELECT * FROM projects WHERE id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            project.setId(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            project.setGroup(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            project.setClient(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            project.setProjectType(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            project.setBillingPartner(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            project.setPartner(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
            project.setManager(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
            project.setNextTask(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
            project.setMemo(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
            project.setRegularDeadline(Date(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9))));
            project.setInternalDeadline(Date(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10))));
            project.setExtended(sqlite3_column_int(stmt, 11) != 0);
            project.setReportType(static_cast<ReportType>(sqlite3_column_int(stmt, 12)));

            sqlite3_finalize(stmt);
            return true;
        }

        sqlite3_finalize(stmt);
        return false;
    }

    // Definition of a method to get projects within a date range; takes two Dates and a ReportType as parameters; returns vector of Projects
    vector<Project> ProjectsDatabase::getProjectsByDateRange(const Date& startDate, const Date& endDate, ReportType reportType) const {
        vector<Project> results;
        string query = "SELECT * FROM projects WHERE ";

        // Determine which deadline to use based on report type
        if (reportType == ReportType::RegularDeadline) {
            query += "regular_deadline BETWEEN ? AND ?";
        } else {
            query += "internal_deadline BETWEEN ? AND ?";
        }

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return results;
        }

        // Bind date parameters
        sqlite3_bind_text(stmt, 1, startDate.getDateStr().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, endDate.getDateStr().c_str(), -1, SQLITE_STATIC);

        // Retrieve and process each matching project
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Project project;
            project.setId(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            project.setGroup(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            project.setClient(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            project.setProjectType(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            project.setBillingPartner(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            project.setPartner(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
            project.setManager(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
            project.setNextTask(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
            project.setMemo(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
            project.setRegularDeadline(Date(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9))));
            project.setInternalDeadline(Date(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10))));
            project.setExtended(sqlite3_column_int(stmt, 11) != 0);
            project.setReportType(static_cast<ReportType>(sqlite3_column_int(stmt, 12)));

            results.push_back(project);
        }

        sqlite3_finalize(stmt);
        return results;
    }

    // Definition of a method to list all projects (for debugging purposes); takes no parameters; returns void
    void ProjectsDatabase::listAllProjects() const {
        // Get all projects from database
        vector<Project> projects = getAllProjects();

        // Print header
        cout << "List of all projects:" << endl;
        cout << "ID | Client | Project Type | Manager | Regular Deadline" << endl;
        cout << "----------------------------------------------------" << endl;

        // Print each project's details
        for (const auto& project : projects) {
            cout << project.getId() << " | "
                      << project.getClient() << " | "
                      << project.getProjectType() << " | "
                      << project.getManager() << " | "
                      << project.getRegularDeadline().getDateStr() << endl;
        }
    }

// PROJECT MANAGER CLASS METHODS:

    // Definition of a method to import projects from a CSV file into database; takes a string and a ReportType as parameters; returns bool
    bool ProjectManager::importFromCSV(const string& filename, ReportType reportType) {

        // Open and validate input file
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            cerr << "Failed to open file: " << filename << endl;
            cerr << "Error: " << strerror(errno) << endl;
            throw runtime_error("Error: Could not open file " + filename);
        }

        // Read and process header line
        string headerLine;
        if (!getline(inFile, headerLine)) {
            cerr << "File is empty" << endl;
            throw runtime_error("Error: File is empty");
        }

        // Update column mappings based on header
        try {
            ProjectsDatabase::updateColumnMappingsFromCSVHeader(headerLine);
        } catch (const exception& e) {
            cerr << "Error updating column mappings: " << e.what() << endl;
            throw;
        }

        // Process CSV data
        vector<Project> csvProjects;
        int lineCount = 0;

        string line;
        while (getline(inFile, line)) {
            lineCount++;

            try {
                // Initialize project and parsing variables
                Project project;
                project.setReportType(reportType);
                int columnNum = 0;
                bool inQuotes = false;
                string currentCell;

                // Remove trailing carriage return if present
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }

                // Parse line character by character
                for (size_t i = 0; i < line.length(); i++) {
                    char ch = line[i];

                    if (ch == '"') {
                        inQuotes = !inQuotes;
                    } else if (ch == ',' && !inQuotes) {
                        project.saveToAppropriateVariable(columnNum, currentCell, reportType);
                        currentCell.clear();
                        columnNum++;
                    } else if (ch != '\r' && ch != '\n') {  // Skip CR/LF characters
                        currentCell += ch;
                    }
                }

                // Process final column
                if (!currentCell.empty() || columnNum < 9) {
                    project.saveToAppropriateVariable(columnNum, currentCell, reportType);
                }

                csvProjects.push_back(project);
            } catch (const exception& e) {
                cerr << "Error processing line " << lineCount << ": " << e.what() << endl;
            }
        }

        inFile.close();

        // Retrieve existing projects from database
        vector<Project> dbProjects;
        try {
            dbProjects = database.getAllProjects();
        } catch (const exception& e) {
            cerr << "Error retrieving projects from database: " << e.what() << endl;
            throw;
        }

        // Prepare list of projects to remove
        vector<string> projectIdsToRemove;
        for (const auto& dbProject : dbProjects) {
            projectIdsToRemove.push_back(dbProject.getId());
        }

        // Track operation counts
        int addedCount = 0, updatedCount = 0, removedCount = 0;
        bool success = true;

        // Compare and update projects
        for (const auto& csvProject : csvProjects) {
            try {
                auto dbProject = find_if(dbProjects.begin(), dbProjects.end(),
                                         [&csvProject](const Project& p) {
                                             return p.getClient() == csvProject.getClient() &&
                                                    p.getProjectType() == csvProject.getProjectType();
                                         });

                if (dbProject == dbProjects.end()) {
                    // Add new project
                    bool addSuccess = database.addProjectToDatabase(csvProject);
                    if (addSuccess) {
                        addedCount++;
                    } else {
                        cerr << "Failed to add new project" << endl;
                        success = false;
                    }
                } else {
                    // Update existing project if needed
                    if (dbProject->getGroup() != csvProject.getGroup() ||
                        dbProject->getBillingPartner() != csvProject.getBillingPartner() ||
                        dbProject->getPartner() != csvProject.getPartner() ||
                        dbProject->getManager() != csvProject.getManager() ||
                        dbProject->getNextTask() != csvProject.getNextTask() ||
                        dbProject->getMemo() != csvProject.getMemo() ||
                        dbProject->getRegularDeadline() != csvProject.getRegularDeadline() ||
                        dbProject->getInternalDeadline() != csvProject.getInternalDeadline() ||
                        dbProject->isExtended() != csvProject.isExtended()) {

                        Project updatedProject = csvProject;
                        updatedProject.setId(dbProject->getId());
                        bool updateSuccess = database.updateProjectInDatabase(updatedProject);
                        if (updateSuccess) {
                            updatedCount++;
                        } else {
                            cerr << "Failed to update project: " << updatedProject.getClient() << " - " << updatedProject.getProjectType() << endl;
                            success = false;
                        }
                    } else {
                    }

                    projectIdsToRemove.erase(remove(projectIdsToRemove.begin(), projectIdsToRemove.end(), dbProject->getId()), projectIdsToRemove.end());
                }
            } catch (const exception& e) {
                cerr << "Error processing project " << csvProject.getClient() << " - " << csvProject.getProjectType() << ": " << e.what() << endl;
                success = false;
            }
        }

        // Remove projects not in CSV
        for (const string& idToRemove : projectIdsToRemove) {
            try {
                bool removeSuccess = database.deleteProjectFromDatabase(idToRemove);
                if (removeSuccess) {
                    removedCount++;
                } else {
                    cerr << "Failed to remove project with ID: " << idToRemove << endl;
                    success = false;
                }
            } catch (const exception& e) {
                cerr << "Error removing project with ID " << idToRemove << ": " << e.what() << endl;
                success = false;
            }
        }

        return success;
    }

    // Definition of a method to search projects in the database based on a search term; takes a string as a parameter; returns a vector of Projects
    vector<Project> ProjectManager::searchProjects(const string& searchTerm) const {
        return database.searchProjects(searchTerm);
    }

    // Definition of a method to export projects to a CSV file; takes a string and a ReportType as parameters; returns void
    void ProjectManager::exportToCSV(const string& filename, ReportType reportType) const {
        // Open output file
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            throw runtime_error("Error: Could not open file for writing: " + filename);
        }

        // Write CSV header
        outFile << "\"Group\",\"Client\",\"Project\",\"Due Date\",\"Billing Partner\",\"Partner\",\"Manager\",\"Next Task\",\"Memo\"\n";

        // Get filtered projects
        vector<Project> filteredProjects = getFilteredProjects();

        // Write each project to CSV
        for (const auto& project : filteredProjects) {
            if (project.getReportType() == reportType) {
                outFile << "\"" << project.getGroup() << "\","
                        << "\"" << project.getClient() << "\","
                        << "\"" << project.getProjectType() << "\","
                        << "\"" << (reportType == ReportType::RegularDeadline ?
                                    project.getRegularDeadline().getDateStr() :
                                    project.getInternalDeadline().getDateStr()) << "\","
                        << "\"" << project.getBillingPartner() << "\","
                        << "\"" << project.getPartner() << "\","
                        << "\"" << project.getManager() << "\","
                        << "\"" << project.getNextTask() << "\","
                        << "\"" << project.getMemo() << "\"\n";
            }
        }

        outFile.close();
    }

// Definition of a method to set a filter for project retrieval based on the specified filter type and value; takes a FilterType and a string as parameters; returns void
    void ProjectManager::setFilter(FilterType filterType, const string& value) {
        // Set appropriate filter based on filter type
        switch (filterType) {
            // Basic text filters
            case FilterType::Group:
                currentFilter.group = value;
                break;
            case FilterType::ProjectType:
                currentFilter.projectType = value;
                break;
            case FilterType::BillingPartner:
                currentFilter.billingPartner = value;
                break;
            case FilterType::Partner:
                currentFilter.partner = value;
                break;
            case FilterType::Manager:
                currentFilter.manager = value;
                break;
            case FilterType::NextTask:
                currentFilter.nextTask = value;
                break;

                // Date filters
            case FilterType::StartDate:
                currentFilter.startDate = Date(value);
                break;
            case FilterType::EndDate:
                currentFilter.endDate = Date(value);
                break;
            case FilterType::RegularDeadline:
                currentFilter.regularDeadline = Date(value);
                break;
            case FilterType::InternalDeadline:
                currentFilter.internalDeadline = Date(value);
                break;

                // Report type filter
            case FilterType::ReportType:
                if (value == "Regular") {
                    currentFilter.reportType = ReportType::RegularDeadline;
                } else if (value == "Internal") {
                    currentFilter.reportType = ReportType::InternalDeadline;
                } else {
                    throw runtime_error("Invalid ReportType value: " + value);
                }
                break;

                // Extended status filter
            case FilterType::Extended:
                if (value == "1" || value == "true" || value == "True" ||
                    value == "TRUE" || value == "yes" || value == "Yes" ||
                    value == "YES" || value == "Extended") {
                    currentFilter.extended = true;
                } else if (value == "0" || value == "false" || value == "False" ||
                           value == "FALSE" || value == "no" || value == "No" ||
                           value == "NO") {
                    currentFilter.extended = false;
                } else {
                    throw runtime_error("Invalid Extended value: " + value);
                }
                break;
        }
    }

    // Definition of a method to set a date range filter for project retrieval; takes two strings as parameters; returns void
    void ProjectManager::setDateRangeFilter(const string& startDate, const string& endDate) {
        currentFilter.startDate = Date(startDate);
        currentFilter.endDate = Date(endDate);
    }

    // Definition of a method to clear a specific filter type; takes a FilterType as a parameter; returns void
    void ProjectManager::clearFilter(FilterType filterType) {
        // Reset specified filter to nullopt based on filter type
        switch (filterType) {
            // Basic text filters
            case FilterType::Group:
                currentFilter.group = nullopt;
                break;
            case FilterType::ProjectType:
                currentFilter.projectType = nullopt;
                break;
            case FilterType::BillingPartner:
                currentFilter.billingPartner = nullopt;
                break;
            case FilterType::Partner:
                currentFilter.partner = nullopt;
                break;
            case FilterType::Manager:
                currentFilter.manager = nullopt;
                break;
            case FilterType::NextTask:
                currentFilter.nextTask = nullopt;
                break;

                // Date filters
            case FilterType::StartDate:
                currentFilter.startDate = nullopt;
                break;
            case FilterType::EndDate:
                currentFilter.endDate = nullopt;
                break;
            case FilterType::RegularDeadline:
                currentFilter.regularDeadline = nullopt;
                break;
            case FilterType::InternalDeadline:
                currentFilter.internalDeadline = nullopt;
                break;

                // Type filters
            case FilterType::ReportType:
                currentFilter.reportType = nullopt;
                break;
            case FilterType::Extended:
                currentFilter.extended = nullopt;
                break;
        }
    }

    // Definition of a method to reset all filters to their default state; takes no parameters; returns void
    void ProjectManager::resetFilter() {
        currentFilter = FilterCriteria();
    }

    // Definition of a method to extract the billing partner from a cell value; takes a string as a parameter; returns a string
    string ProjectManager::getBillingPartner(const string &cellVal) {
        string temp;

        for (int i = 0; i < cellVal.size(); i++) {
            char c = cellVal[i];
            if (c == ',' || i == cellVal.size() - 1) {
                if (i == cellVal.size() - 1) {
                    temp += c;
                }
                for (int i = 0; i < billingPartners.size(); i++) {
                    if (temp.find(billingPartners[i]) != string::npos) {
                        return temp;
                    }
                }
                temp.clear();
            } else {
                temp += c;
            }
        }
        return "";
    }

    // Definition of a method to check if a project is extended based on its tags; takes a string as parameter; returns bool
    bool ProjectManager::isProjectExtended(const string& cellVal) {
        string temp;

        // Process each character to build and check individual tags
        for (int i = 0; i < cellVal.size(); i++) {
            char c = cellVal[i];
            if (c == ',' || i == cellVal.size() - 1) {
                // Handle last character of the last tag
                if (i == cellVal.size() - 1) {
                    temp += c;
                }

                // Look for "Extended" in the current tag
                if (temp.find("Extended") != string::npos) {
                    return true;
                }
                temp.clear();
            } else {
                temp += c;
            }
        }
        return false;
    }

    // Definition of a method to display filtered data for testing purposes; takes a vector of Projects as a parameter; returns void
    void ProjectManager::displayFilteredData(const vector<Project>& projects) const {
        if (projects.empty()) {
            cout << "No data matches the current filter criteria." << endl;
            return;
        }

        for (const auto& project : projects) {
            cout << "Group: " << project.getGroup()
                 << ", Client: " << project.getClient()
                 << ", Project: " << project.getProjectType()
                 << ", Due Date: " << project.getRegularDeadline().getDateStr()
                 << ", Billing Partner: " << project.getBillingPartner()
                 << ", Partner: " << project.getPartner()
                 << ", Manager: " << project.getManager()
                 << ", Next Task: " << project.getNextTask()
                 << ", Memo: " << project.getMemo() << endl;
        }
    }

    // Definition of a method to filter projects by a date range; takes two Dates and a ReportType as parameters; returns a vector of Projects
    vector<Project> ProjectManager::filterByDateRange(const Date& startDate, const Date& endDate, ReportType reportType) const {
        return database.getProjectsByDateRange(startDate, endDate, reportType);
    }

    // Definition of a method to retrieve projects that match the current filter criteria; takes no parameters; returns a vector of Projects
    vector<Project> ProjectManager::getFilteredProjects() const {
        vector<Project> projects = database.getAllProjects();

        if (currentFilter.group) {
            projects.erase(remove_if(projects.begin(), projects.end(),
                                     [this](const Project& project) {
                                         return project.getGroup() != *currentFilter.group;
                                     }), projects.end());
        }
        if (currentFilter.projectType) {
            projects.erase(remove_if(projects.begin(), projects.end(),
                                     [this](const Project& project) {
                                         return project.getProjectType() != *currentFilter.projectType;
                                     }), projects.end());
        }
        if (currentFilter.billingPartner) {
            projects.erase(remove_if(projects.begin(), projects.end(),
                                     [this](const Project& project) {
                                         return project.getBillingPartner() != *currentFilter.billingPartner;
                                     }), projects.end());
        }
        if (currentFilter.partner) {
            projects.erase(remove_if(projects.begin(), projects.end(),
                                     [this](const Project& project) {
                                         return project.getPartner() != *currentFilter.partner;
                                     }), projects.end());
        }
        if (currentFilter.manager) {
            projects.erase(remove_if(projects.begin(), projects.end(),
                                     [this](const Project& project) {
                                         return project.getManager() != *currentFilter.manager;
                                     }), projects.end());
        }
        if (currentFilter.nextTask) {
            projects.erase(remove_if(projects.begin(), projects.end(),
                                     [this](const Project& project) {
                                         return project.getNextTask() != *currentFilter.nextTask;
                                     }), projects.end());
        }
        if (currentFilter.startDate && currentFilter.endDate) {
            projects.erase(remove_if(projects.begin(), projects.end(),
                                     [this](const Project& project) {
                                         Date dueDate = project.getRegularDeadline(); // or getInternalDeadline() based on context
                                         return dueDate < *currentFilter.startDate || dueDate > *currentFilter.endDate;
                                     }), projects.end());
        }
        if (currentFilter.regularDeadline) {
            projects.erase(remove_if(projects.begin(), projects.end(),
                                     [this](const Project& project) {
                                         return project.getRegularDeadline() != *currentFilter.regularDeadline;
                                     }), projects.end());
        }
        if (currentFilter.internalDeadline) {
            projects.erase(remove_if(projects.begin(), projects.end(),
                                     [this](const Project& project) {
                                         return project.getInternalDeadline() != *currentFilter.internalDeadline;
                                     }), projects.end());
        }
        if (currentFilter.reportType) {
            projects.erase(remove_if(projects.begin(), projects.end(),
                                     [this](const Project& project) {
                                         return project.getReportType() != *currentFilter.reportType;
                                     }), projects.end());
        }
        if (currentFilter.extended) {  // Add this block
            projects.erase(remove_if(projects.begin(), projects.end(),
                                     [this](const Project& project) {
                                         return project.isExtended() != *currentFilter.extended;
                                     }), projects.end());
        }

        return projects;
    }

    // Definition of a method to update column mappings based on CSV header; takes a string as parameter; returns void
    void ProjectsDatabase::updateColumnMappingsFromCSVHeader(const string& headerLine) {
        vector<string> headers;
        string currentHeader;

        // Initialize all column indices to -1
        GROUP_COLUMN = CLIENT_COLUMN = PROJECT_COLUMN = DUE_DATE_COLUMN =
        BILLING_PARTNER_COLUMN = PARTNER_COLUMN = MANAGER_COLUMN =
        NEXT_TASK_COLUMN = MEMO_COLUMN = -1;

        // Parse header line character by character
        for (size_t i = 0; i < headerLine.length(); i++) {
            char c = headerLine[i];
            if (c == ',' || c == '\n' || c == '\r') {
                headers.push_back(currentHeader);
                currentHeader.clear();
            } else {
                currentHeader += c;
            }
        }

        // Handle last header if no trailing delimiter
        if (!currentHeader.empty()) {
            headers.push_back(currentHeader);
        }

        // Map column indices based on header names
        for (size_t i = 0; i < headers.size(); i++) {
            const string& h = headers[i];
            if (h == "Client") CLIENT_COLUMN = i;
            else if (h == "Project") PROJECT_COLUMN = i;
            else if (h == "Tags") BILLING_PARTNER_COLUMN = i;
            else if (h == "Memo") MEMO_COLUMN = i;
            else if (h == "Due Date") DUE_DATE_COLUMN = i;
            else if (h == "Client Group(s)") GROUP_COLUMN = i;
            else if (h == "Next Task") NEXT_TASK_COLUMN = i;
            else if (h == "Manager") MANAGER_COLUMN = i;
            else if (h == "Partner") PARTNER_COLUMN = i;
        }
    }

    // Definition of a method to get all projects from database; takes no parameters; returns vector of Projects
    vector<Project> ProjectManager::getAllProjects() {
        return database.getAllProjects();
    }

} // namespace TaxReturnSystem