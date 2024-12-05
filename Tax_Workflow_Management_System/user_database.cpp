/**
 * @file user_database.cpp
 * @brief Implementation of the UserDatabase class for managing user data
 *
 * This file implements database operations for user management including:
 * - Database connection management
 * - User CRUD operations
 * - Password and role management
 * - Email management
 */

#include "user_database.h"

using namespace std;

namespace TaxReturnSystem {

    // Definition of UserDatabase constructor; takes database path as parameter; initializes database connection
    UserDatabase::UserDatabase(const string& path) : dbPath(path), db(nullptr) {
        // Log constructor start
        cout << "UserDatabase constructor called with path: " << path << endl;

        // Open database connection
        if (!openDatabase()) {
            throw runtime_error("Failed to open database");
        }

        // Create necessary tables
        cout << "Database opened successfully. Attempting to create tables..." << endl;
        createTablesIfNotExist();

        // Log successful initialization
        cout << "UserDatabase constructor completed successfully." << endl;
    }

    // Destructor; closes the database connection
    UserDatabase::~UserDatabase() {
        closeDatabase();
    }

    // Definition of a method to open the database connection; returns bool indicating success
    bool UserDatabase::openDatabase() {
        // Log attempt to open database
        cout << "Attempting to open database at: " << dbPath << endl;

        // Open the database connection
        int result = sqlite3_open(dbPath.c_str(), &db);
        if (result != SQLITE_OK) {
            // Log error if opening fails
            cerr << "Failed to open database: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Log success
        cout << "Database opened successfully." << endl;
        return true;
    }

    // Definition of a method to close the database connection; no parameters or return value
    void UserDatabase::closeDatabase() {
        // Close database if connection exists
        if (db) {
            sqlite3_close(db);
            db = nullptr;
        }
    }

    // Definition of a method to create database tables if they don't exist; no parameters or return value
    void UserDatabase::createTablesIfNotExist() {
        // Log method entry
        cout << "Entering createTablesIfNotExist()" << endl;

        // SQL statement to create users table with required fields
        const char* sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            passwordHash TEXT NOT NULL,
            email TEXT NOT NULL,
            user_role INTEGER NOT NULL,
            last_password_change INTEGER NOT NULL
        );
    )";

        // Execute SQL statement
        char* errMsg = nullptr;
        cout << "Executing SQL to create users table..." << endl;
        int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

        // Handle any SQL errors
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            string errorMsg = errMsg ? errMsg : "Unknown error";
            sqlite3_free(errMsg);
            throw runtime_error("Failed to create users table: " + errorMsg);
        }

        // Log successful creation
        cout << "Users table created successfully." << endl;
    }

    // Definition of a method to add a new user to the database; takes username, passwordHash, and email as parameters; returns bool indicating success
    bool UserDatabase::addUserToDatabase(const string& username, const string& passwordHash, const string& email) {
        // Log attempt to add user
        cout << "Attempting to add user: " << username << endl;

        // SQL statement to insert a new user
        string sql = "INSERT INTO users (username, passwordHash, email, user_role, last_password_change) VALUES (?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        // Prepare SQL statement
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Get current time for last_password_change
        auto now = chrono::system_clock::now();
        auto nowTime = chrono::system_clock::to_time_t(now);

        // Bind parameters to SQL statement
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, static_cast<int>(UserRole::Manager)); // Default role
        sqlite3_bind_int64(stmt, 5, nowTime);

        // Execute SQL statement
        int result = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        // Check execution result
        if (result != SQLITE_DONE) {
            cerr << "Failed to add user: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Log success
        cout << "User added successfully." << endl;
        return true;
    }

    // Definition of a method to retrieve a user from the database; takes username and output parameters for user and hashedPassword; returns bool indicating success
    bool UserDatabase::getUserFromDatabase(const string& username, User& user, string& hashedPassword) {
        // SQL statement to select user data
        string sql = "SELECT username, passwordHash, email, user_role, last_password_change FROM users WHERE username = ?;";
        sqlite3_stmt* stmt;

        // Prepare SQL statement
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Bind username parameter
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        // Execute and retrieve user data if found
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // Populate user object with database values
            user.setUsername(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            hashedPassword = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            user.setEmail(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            user.setRole(static_cast<UserRole>(sqlite3_column_int(stmt, 3)));
            user.setLastPasswordChange(chrono::system_clock::from_time_t(sqlite3_column_int64(stmt, 4)));

            sqlite3_finalize(stmt);
            return true;
        }

        // Clean up and return false if user not found
        sqlite3_finalize(stmt);
        return false;
    }

    // Definition of a method to retrieve a username from an email; takes email and output parameter for username; returns bool indicating success
    bool UserDatabase::getUsernameFromEmail(const string& email, string& username) {
        // SQL statement to select username based on email
        string sql = "SELECT username FROM users WHERE email = ?;";
        sqlite3_stmt* stmt;

        // Prepare SQL statement
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Bind email parameter
        sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);

        // Execute and retrieve username if found
        bool found = false;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            found = true;
        }

        // Clean up and return result
        sqlite3_finalize(stmt);
        return found;
    }

    // Definition of a method to retrieve a user's email from the database; takes username and output parameter for email; returns bool indicating success
    bool UserDatabase::getUserEmailFromDatabase(const string& username, string& email) {
        // SQL statement to select email based on username
        string sql = "SELECT email FROM users WHERE username = ?;";
        sqlite3_stmt* stmt;

        // Prepare SQL statement
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Bind username parameter
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        // Execute and retrieve email if found
        bool success = false;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // Safely convert and store email if it exists
            const char* emailChar = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            if (emailChar) {
                email = string(emailChar);
                success = true;
            }
        }

        // Clean up and return result
        sqlite3_finalize(stmt);
        return success;
    }

    // Definition of a method to update a user's information in the database; takes User object and hashedPassword; returns bool indicating success
    bool UserDatabase::updateUserInDatabase(const User& user, const string& hashedPassword) {
        // Determine SQL statement based on whether password needs updating
        string sql;
        sqlite3_stmt* stmt;

        if (hashedPassword.empty()) {
            sql = "UPDATE users SET user_role = ?, last_password_change = ?, email = ? WHERE username = ?;";
        } else {
            sql = "UPDATE users SET user_role = ?, last_password_change = ?, email = ?, passwordHash = ? WHERE username = ?;";
        }

        // Prepare SQL statement
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Bind parameters to SQL statement
        sqlite3_bind_int(stmt, 1, static_cast<int>(user.getRole()));
        sqlite3_bind_int64(stmt, 2, chrono::system_clock::to_time_t(user.getLastPasswordChange()));
        sqlite3_bind_text(stmt, 3, user.getEmail().c_str(), -1, SQLITE_STATIC);

        if (!hashedPassword.empty()) {
            sqlite3_bind_text(stmt, 4, hashedPassword.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 5, user.getUsername().c_str(), -1, SQLITE_STATIC);
        } else {
            sqlite3_bind_text(stmt, 4, user.getUsername().c_str(), -1, SQLITE_STATIC);
        }

        // Execute SQL statement and finalize
        int result = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        // Return success status
        return result == SQLITE_DONE;
    }

    // Definition of a method to update a user's password; takes username and newPasswordHash as parameters; returns bool indicating success
    bool UserDatabase::updateUserPasswordInDatabase(const string& username, const string& newPasswordHash) {
        // SQL statement to update password and last change time
        string sql = "UPDATE users SET hashed_password = ?, last_password_change = ? WHERE username = ?;";
        sqlite3_stmt* stmt;

        // Prepare SQL statement
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Get current time and bind parameters
        auto now = chrono::system_clock::now();
        sqlite3_bind_text(stmt, 1, newPasswordHash.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 2, chrono::system_clock::to_time_t(now));
        sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_STATIC);

        // Execute SQL statement and finalize
        int result = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        // Return success status
        return result == SQLITE_DONE;
    }

    // Definition of a method to update a user's email; takes username and newEmail as parameters; returns bool indicating success
    bool UserDatabase::updateUserEmailInDatabase(const string& username, const string& newEmail) {
        // SQL statement to update email
        string sql = "UPDATE users SET email = ? WHERE username = ?;";
        sqlite3_stmt* stmt;

        // Prepare SQL statement
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Bind parameters
        sqlite3_bind_text(stmt, 1, newEmail.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);

        // Execute SQL statement and finalize
        int result = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        // Return success status
        return result == SQLITE_DONE;
    }

    // Definition of a method to update a user's role; takes username and newRole as parameters; returns bool indicating success
    bool UserDatabase::updateUserRoleInDatabase(const string& username, UserRole newRole) {
        // SQL statement to update user role
        string sql = "UPDATE users SET user_role = ? WHERE username = ?;";
        sqlite3_stmt* stmt;

        // Prepare SQL statement
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Bind parameters
        sqlite3_bind_int(stmt, 1, static_cast<int>(newRole));
        sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);

        // Execute SQL statement and finalize
        int result = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        // Return success status
        return result == SQLITE_DONE;
    }

    // Definition of a method to list all users in the database; no parameters; no return value
    void UserDatabase::listAllUsers() const {
        // SQL statement to select all user information
        const char* sql = "SELECT username, email, user_role FROM users;";
        sqlite3_stmt* stmt;

        // Prepare SQL statement
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return;
        }

        // Print header for user list
        cout << "List of all users:" << endl;
        cout << "Username | Email | Role" << endl;
        cout << "------------------------" << endl;

        // Iterate through all users and print their information
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            const char* email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int role = sqlite3_column_int(stmt, 2);

            cout << username << " | " << email << " | " << role << endl;
        }

        // Clean up prepared statement
        sqlite3_finalize(stmt);
    }

} // namespace TaxReturnSystem