#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <unordered_set>
#include <thread>
#include <cstdlib>
#include <optional>
#include <random>
#include <openssl/sha.h>
#include <sqlite3.h>
#include <bcrypt/BCrypt.hpp>
#include "jwt-cpp/jwt.h"
#include "config.h"
#include "logger.h"
#include "email_service.h"
#include "user_database.h"
#include "user.h"

using namespace std;

namespace TaxReturnSystem {

    // Structure to hold user login attempts
    struct LoginAttempt {
        int attempts; // Number of login attempts
        chrono::system_clock::time_point lockoutUntil; // Time until the user is locked out
    };

    enum class LoginResult { Success, InvalidUsername, InvalidPasswordLength, AccountLocked, InvalidCredentials };

    // Class for handling authentication processes
    class Auth {
    private:
        // Service and utility objects
        EmailService& emailService; // Reference to email service
        UserDatabase& userDatabase; // Reference to user database
        Logger& logger; // Reference to logger

        // Authentication-related data
        string secretKey; // JWT secret key
        mutex loginMutex; // Mutex for thread safety

        // Data structures for tracking user activities
        unordered_map<string, LoginAttempt> loginAttempts; // Track login attempts per user
        unordered_map<string, pair<string, chrono::system_clock::time_point>> resetTokens; // Map of reset tokens
        unordered_map<string, vector<chrono::system_clock::time_point>> resetAttempts; // Track reset attempts
        unordered_map<string, chrono::system_clock::time_point> lastPasswordChange; // Track last password change
        unordered_map<string, chrono::system_clock::time_point> lastUsernameRecovery; // Track last username recovery

        // Token generation and management
        string generateToken(const string& username) const; // Generate JWT token
        string generateResetToken() const; // Generate reset token

        // Password-related methods
        string hashPassword(const string& password); // Hash a password
        bool verifyPassword(const string& password, const string& hash) const; // Verify password against hash
        bool sendResetEmail(const string& email, const string& token) const; // Send password reset email

        // Input validation methods
        bool isValidUsername(const string& username) const; // Check if username is valid
        bool isStrongPassword(const string& password) const; // Check if password is strong
        bool isValidPasswordLength(const string& password) const; // Check if password length is valid

        // Rate limiting and security methods
        bool isResetRateLimited(const string& username); // Check if reset is rate limited
        void recordResetAttempt(const string& username); // Record a reset attempt
        bool canChangePassword(const string& username) const; // Check if password can be changed
        bool canRecoverUsername(const string& email) const; // Check if username can be recovered
        bool isAccountLocked(const string& username) const; // Check if account is locked
        void resetLoginAttempts(const string& username); // Reset login attempts
        void incrementLoginAttempts(const string& username); // Increment login attempts

        unordered_set<string> invalidatedTokens; // Set of invalidated tokens

    public:
        // Constructor and Destructor
        Auth(EmailService& emailService, UserDatabase& userDatabase, Logger& logger, const string& jwtSecretKey); // Constructor
        ~Auth(); // Destructor

        // User management methods
        bool registerUser(const string& username, const string& password, const string& email); // Register a new user
        LoginResult loginUser(const string& username, const string& password); // Log in a user
        bool validateToken(const string& token) const; // Validate a JWT token
        void invalidateToken(const string& token) { invalidatedTokens.insert(token); } // Invalidate a token
        User getUserFromToken(const string& token) const; // Get user from token

        // Password management methods
        bool requestPasswordReset(const string& username); // Request password reset
        bool resetPassword(const string& username, const string& token, const string& newPassword); // Reset password
        bool changePassword(const string& username, const string& oldPassword, const string& newPassword); // Change password
        bool updateUserPassword(User& user, const string& newPassword); // Update user's password

        // Account recovery methods
        bool recoverUsername(const string& email); // Recover username

        string createTokenForUser(const string& username) { return generateToken(username); } // Create token for user
    };

} // namespace TaxReturnSystem