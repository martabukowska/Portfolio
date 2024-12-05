#pragma once

#include <string>
#include <sqlite3.h>
#include <iostream>
#include <chrono>
#include "user.h"

namespace TaxReturnSystem {

    class UserDatabase {
    private:
        string dbPath; // Path to the database
        sqlite3* db; // Pointer to the SQLite database connection

        // Methods for database connection management
        bool openDatabase(); // Opens the database connection
        void closeDatabase(); // Closes the database connection

    public:
        // Constructor and destructor
        UserDatabase(const string& path); // Constructor: Initializes the database connection
        ~UserDatabase(); // Destructor: Closes the database connection

        void createTablesIfNotExist(); // Creates necessary tables if they don't exist

        // Methods for user data management
        bool addUserToDatabase(const string& username, const string& passwordHash, const string& email); // Adds a new user to the database
        bool getUserFromDatabase(const string& username, User& user, string& hashedPassword); // Retrieves a user from the database
        bool getUsernameFromEmail(const string& email, string& username); // Retrieves a username from an email
        bool getUserEmailFromDatabase(const string& username, string& email); // Retrieves a user's email from the database

        // Methods for updating user information
        bool updateUserInDatabase(const User& user, const string& hashedPassword = ""); // Updates a user's information in the database
        bool updateUserPasswordInDatabase(const string& username, const string& newPasswordHash); // Updates a user's password in the database
        bool updateUserEmailInDatabase(const string& username, const string& newEmail); // Updates a user's email in the database
        bool updateUserRoleInDatabase(const string& username, UserRole newRole); // Updates a user's role in the database

        void listAllUsers() const; // Lists all users in the database
        string getDbPath() const { return dbPath; } // Returns the database path
    };

} // namespace TaxReturnSystem