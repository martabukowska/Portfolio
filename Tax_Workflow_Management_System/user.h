#pragma once

#include <string>
#include <chrono>

using namespace std;

namespace TaxReturnSystem {

    // Enum for different user roles in the system
    enum class UserRole {
        Admin, BillingPartner, Partner, Manager
    };

    class User {
    private:
        // User data members
        string username; // User's username
        string hashedPassword; // User's hashed password
        string email; // User's email
        UserRole userRole; // User's role
        chrono::system_clock::time_point lastPasswordChange; // Timestamp of last password change

        string getHashedPassword() const; // Get the hashed password

    public:
        // Constructors
        User() : userRole(UserRole::Manager) {}; // Default constructor with Manager role
        User(const string &username, const string &password, const string &email); // Parameterized constructor

        // Getter methods
        string getUsername() const; // Get the username
        UserRole getRole() const; // Get the user's role
        string getEmail() const; // Get the user's email
        chrono::system_clock::time_point getLastPasswordChange() const; // Get timestamp of last password change

        // Setter methods
        void setUsername(const string &name); // Set the username
        void setRole(const UserRole &role); // Set the user's role
        void setEmail(const string &newEmail); // Set the user's email
        void setLastPasswordChange(const chrono::system_clock::time_point time); // Set timestamp of last password change

        friend class Auth; // Allow Auth class to access private members
    };

} // namespace TaxReturnSystem