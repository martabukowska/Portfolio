/**
 * @file user.cpp
 * @brief Implementation of the User class for the Tax Return System
 *
 * This file provides the implementation of the User class, which includes
 * methods for accessing and modifying user attributes such as username,
 * role, email, and password change information.
 */

#include "user.h"

using namespace std;

namespace TaxReturnSystem {

    // Definition of method to get the hashed password; returns passwordHash
    string User::getHashedPassword() const {
        return hashedPassword;
    }

    // Definition of method to get the user's username; returns username
    string User::getUsername() const {
        return username;
    }

    // Definition of method to get the user's role; returns userRole
    UserRole User::getRole() const {
        return userRole;
    }

    // Definition of method to get the user's email; returns email
    string User::getEmail() const {
        return email;
    }

    // Definition of method to get the last password change; returns lastPasswordChange
    chrono::system_clock::time_point User::getLastPasswordChange() const {
        return lastPasswordChange;
    }


    // Definition of method to set the user's username; takes name as parameter
    void User::setUsername(const string &name) {
        username = name;
    }

    // Definition of method to set the user's role; takes role as parameter
    void User::setRole(const UserRole &role) {
        userRole = role;
    }

    // Definition of method to set the user's email; takes email as parameter
    void User::setEmail(const string& email) {
        this->email = email;
    }

    // Definition of method to set the last password change; takes time point as parameter
    void User::setLastPasswordChange(const chrono::system_clock::time_point time) {
        lastPasswordChange = time;
    }

}