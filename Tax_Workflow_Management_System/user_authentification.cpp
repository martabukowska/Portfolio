/**
 * @file user_authentification.cpp
 * @brief Implementation of authentication-related functionality
 *
 * This file implements the Auth class which handles:
 * - User authentication (login, logout)
 * - Password management (reset, change)
 * - Token generation and validation
 * - Security measures (rate limiting, account locking)
 */

#include "user_authentification.h"

using namespace std;

namespace TaxReturnSystem {

    // AUTH CLASS METHODS:

    // Definition of method to generate a JWT token for a given username; takes username as parameter, returns token
    string Auth::generateToken(const string& username) const {
        if (secretKey.empty()) { // If the key is empty, log the error and return an empty string
            logger.log("JWT secret key is empty");
            cerr << "JWT secret key is empty!" << endl;
            return ""; // Return an empty string to indicate failure in token generation
        }
        // If the secret key is available
        auto token = jwt::create() // Create a new JWT token
                .set_issuer(JWT_ISSUER) // Set the issuer of the token
                .set_subject(username) // Set the subject (username) of the token
                .set_expires_at(chrono::system_clock::now() + chrono::seconds(TOKEN_EXPIRY)) // Set the token to expire
                .sign(jwt::algorithm::hs256{secretKey}); // Sign the token using the stored secret key
        logger.log("Token generated for user: " + username);
        return token; // Return the generated token
    }

    // Definition of method to generate a reset token for password recovery; takes no parameters, returns a reset token
    string Auth::generateResetToken() const {
        const string& chars = TOKEN_CHARS;
        random_device rd;
        string token;
        for (int i = 0; i < RESET_TOKEN_LENGTH; i++) {
            token += chars[rd() % chars.size()];
        }
        return token;
    }

    // Definition of method to hash a password using bcrypt; takes password as parameter, returns hashed password
    string Auth::hashPassword(const string& password) {
        return BCrypt::generateHash(password); // Hash the provided password with bcrypt
    }

    // Definition of method to verify a password against a stored hash using BCrypt; takes plain text password and hash as parameters, returns true if password matches hash, false otherwise
    bool Auth :: verifyPassword(const string& password, const string& hash) const {
        return BCrypt::validatePassword(password, hash);
    }

    //TODO: resolve the email issues

    // Definition of method to send a password reset email; takes email and token as parameters, returns true if email sent successfully, false otherwise
    bool Auth::sendResetEmail(const string& email, const string& token) const {
        string subject = "Password Reset Request";
        string body = "Your password reset token is: " + token + "\n"
                       "This token will expire in " + to_string(RESET_TOKEN_VALIDITY / 60) + " minutes.\n"
                       "If you did not request this reset, please ignore this email.";

        bool result = emailService.sendEmail(email, subject, body);
        if (result) {
            logger.log("Password reset email sent to: " + email);
        } else {
            logger.log("Failed to send password reset email to: " + email);
        }
        return result;
    }

    // Definition of method to validate username; takes username as parameter, returns true if username is valid, false otherwise
    bool Auth::isValidUsername(const string& username) const {
        if (username.length() < MIN_USERNAME_LENGTH || username.length() > MAX_USERNAME_LENGTH) {
            return false;
        }
        // Username should only contain alphanumeric characters and underscores
        for (char c : username) {
            if (!isalnum(c) && c != '_') {
                return false;
            }
        }
        return true;
    }

    // Definition of method to check if password is strong; takes password as parameter, returns true if it meets all the criteria, false otherwise
    bool Auth::isStrongPassword(const string& password) const {
        if (!isValidPasswordLength(password)) {
            return false;
        }
        bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
        for (char c : password) {
            if (isupper(c)) {
                hasUpper = true;
            } else if (islower(c)) {
                hasLower = true;
            } else if (isdigit(c)) {
                hasDigit = true;
            } else {
                hasSpecial = true;
            }
        }
        return hasUpper && hasLower && hasDigit && hasSpecial;
    }

    // Definition of method to check if password length is valid; takes password as parameter, returns true if the length is valid, false otherwise
    bool Auth::isValidPasswordLength(const string& password) const {
        return password.length() >= MIN_PASSWORD_LENGTH && password.length() <= MAX_PASSWORD_LENGTH;
    }

    // Definition of method to check if password reset attempts are rate limited; takes username as parameter, returns true if rate limited, false otherwise
    bool Auth::isResetRateLimited(const string& username)  {
        auto now = chrono::system_clock::now();
        auto& attempts = resetAttempts[username];

        // Remove attempts older than the window
        attempts.erase(
                remove_if(attempts.begin(), attempts.end(),
                          [now](const auto& attempt) {
                              return now - attempt > chrono::seconds(RESET_ATTEMPT_WINDOW);
                          }),
                attempts.end()
        );

        return attempts.size() >= MAX_RESET_ATTEMPTS;
    }

    // Definition of method to record a password reset attempt; takes username as parameter
    void Auth::recordResetAttempt(const string& username) {
        resetAttempts[username].push_back(chrono::system_clock::now());
    }

    // Definition of method to check if a user can change their password; takes username as parameter, returns true if allowed, false otherwise
    bool Auth::canChangePassword(const string& username) const {
        auto it = lastPasswordChange.find(username);
        if (it == lastPasswordChange.end()) {
            return true;
        }
        return chrono::system_clock::now() - it->second > chrono::seconds(PASSWORD_CHANGE_COOLDOWN);
    }

    bool Auth::updateUserPassword(User& user, const string& newPassword) {
        string hashedNewPassword = hashPassword(newPassword);
        user.setLastPasswordChange(chrono::system_clock::now());
        return userDatabase.updateUserInDatabase(user, hashedNewPassword);
    }

    // Definition of method to check if a user can recover their username; takes email as parameter, returns true if allowed, false otherwise
    bool Auth::canRecoverUsername(const string& email) const {
        auto it = lastUsernameRecovery.find(email);
        if (it == lastUsernameRecovery.end()) {
            return true;
        }
        return chrono::system_clock::now() - it->second > chrono::seconds(USERNAME_RECOVERY_COOLDOWN);
    }

    // Definition of method to check if a user account is locked; takes username as parameter, returns true if the account is locked, false otherwise
    bool Auth::isAccountLocked(const string& username) const {
        // Check if the loginAttempts map contains the entry for the specified username
        auto iter = loginAttempts.find(username);
        if (iter != loginAttempts.end()) { // If an entry for the given username was found
            if (iter->second.lockoutUntil > chrono::system_clock::now()) { // If the current time is less than the lockoutUntil time, the user is locked out
                return true;
            }
        }
        return false;
    }

    // Definition of method to reset login attempts for a user; takes username as parameter
    void Auth::resetLoginAttempts(const string& username) {
        loginAttempts.erase(username);
    }

    // Definition of method to increment login attempts for a user; takes username as parameter, increments the attempt count, and locks the account if maximum attempts are reached; returns void
    void Auth::incrementLoginAttempts(const string& username) {
        auto& attempts = loginAttempts[username];
        attempts.attempts++; // Increase the number of attempts for this user
        cout << "Incrementing failed attempts. New count: " << attempts.attempts << endl;
        if (attempts.attempts >= MAX_LOGIN_ATTEMPTS) { // If the number of attempts is greater than or equal to the maximum allowed attempts
            // Lockout the user by setting the lockoutUntil time to the current time plus the lockout duration
            attempts.lockoutUntil = chrono::system_clock::now() + chrono::seconds(LOCKOUT_DURATION);
            logger.log("User locked out due to multiple failed attempts: " + username);
            cout << "User locked out. Lockout until: " << attempts.lockoutUntil.time_since_epoch().count() << endl;
        }
    }

    // Constructor
    Auth::Auth(EmailService& emailService, UserDatabase& userDatabase, Logger& logger, const string& jwtSecretKey)
            : emailService(emailService), userDatabase(userDatabase), logger(logger), secretKey(jwtSecretKey) {

        if (secretKey.empty()) {
            string errorMsg = "JWT secret key is empty!";
            cerr << errorMsg << endl;
            logger.log(errorMsg);
            throw runtime_error(errorMsg);
        }

        logger.log("Auth object initialized");
    }

    // Destructor
    Auth::~Auth() {}

    // Definition of method to register a user in the database; takes username and password as parameters, returns true if successful, false otherwise
    bool Auth::registerUser(const string& username, const string& password, const string& email) {
        if (!isValidUsername(username)) {
            logger.log("Failed registration attempt: Invalid username format for " + username);
            cerr << "Invalid username. Username must be " << MIN_USERNAME_LENGTH << "-" << MAX_USERNAME_LENGTH
                 << " characters long and contain only alphanumeric characters and underscores." << endl;
            return false;
        }

        if (!isStrongPassword(password)) {
            logger.log("Failed registration attempt: Weak password for " + username);
            cerr << "Password is not strong enough. It must be " << MIN_PASSWORD_LENGTH << "-" << MAX_PASSWORD_LENGTH
                 << " characters long and contain at least one uppercase letter, one lowercase letter, one digit, and one special character." << endl;
            return false;
        }

        string hashedPassword = hashPassword(password); // Hash the provided password with bcrypt
        bool result = userDatabase.addUserToDatabase(username, hashedPassword, email); // Store the user in the database and return the result

        if (result) {
            logger.log("Successful user registration: " + username);
        } else {
            logger.log("Failed user registration: " + username);
        }

        return result;
    }

    // Definition of method to log in a user; takes username and password as parameters, returns token if successful, empty string otherwise
    LoginResult Auth::loginUser(const string& username, const string& password) {
        lock_guard<mutex> lock(loginMutex);

        cout << "Login attempt for user: " << username << endl;
        logger.log("Login attempt for user: " + username);

        if (!isValidUsername(username)) {
            logger.log("Failed login attempt: Invalid username format for " + username);
            cerr << "Invalid username format." << endl;
            return LoginResult::InvalidUsername;
        }

        if (!isValidPasswordLength(password)) {
            logger.log("Failed login attempt: Invalid password length for " + username);
            cerr << "Invalid password length." << endl;
            return LoginResult::InvalidPasswordLength;
        }

        if (isAccountLocked(username)) {
            logger.log("Login attempt rejected: User locked out - " + username);
            cerr << "User is currently locked out." << endl;
            return LoginResult::AccountLocked;
        }

        User user;
        string hashedPassword;
        bool userExists = userDatabase.getUserFromDatabase(username, user, hashedPassword);

        bool passwordCorrect = userExists && verifyPassword(password, hashedPassword);

        if (passwordCorrect) {
            resetLoginAttempts(username);
            logger.log("Successful login: " + username);
            cout << "Login successful." << endl;
            return LoginResult::Success;
        } else {
            incrementLoginAttempts(username);
            logger.log("Failed login attempt: " + username);
            cout << "Login failed." << endl;
            return LoginResult::InvalidCredentials;
        }
    }

    // Definition of method to validate JWT token; takes token as parameter, returns true if valid, false otherwise
    bool Auth::validateToken(const string& token) const {
        try {
            if (invalidatedTokens.find(token) != invalidatedTokens.end()) {
                return false;
            }
            cout << "Validating token: " << token.substr(0, 10) << "..." << endl; // Log first 10 chars for security
            auto decoded = jwt::decode(token);

            cout << "Token decoded successfully" << endl;

            // Print issuer and subject claims
            cout << "Issuer: " << decoded.get_issuer() << endl;
            cout << "Subject: " << decoded.get_subject() << endl;

            // Check expiration
            if (decoded.has_expires_at()) {
                cout << "Token has expiration claim" << endl;
                auto exp = decoded.get_expires_at();
                if (chrono::system_clock::now() > exp) {
                    cout << "Token has expired" << endl;
                    logger.log("Token validation failed: Token expired");
                    return false;
                }
            } else {
                cout << "Token does not have expiration claim" << endl;
                logger.log("Token validation failed: No expiration claim");
                return false;
            }

            // Verify signature
            auto verifier = jwt::verify()
                    .allow_algorithm(jwt::algorithm::hs256{secretKey})
                    .with_issuer(JWT_ISSUER);

            verifier.verify(decoded);
            cout << "Token signature verified successfully" << endl;

            logger.log("Token validated successfully");
            return true;
        } catch (const exception& e) {
            cerr << "Error validating token: " << e.what() << endl;
            logger.log("Token validation failed: " + string(e.what()));
            return false;
        }
    }

    // Definition of method to request a password reset; takes username as parameter, returns true if reset request was successful, false otherwise
    bool Auth::requestPasswordReset(const string& username) {
        if (isResetRateLimited(username)) {
            logger.log("Password reset rate limited for user: " + username);
            cerr << "Too many reset attempts. Please try again later." << endl;
            return false;
        }

        string email;
        if (!userDatabase.getUserEmailFromDatabase(username, email)) {
            logger.log("Password reset request failed: User not found - " + username);
            cerr << "User not found." << endl;
            return false;
        }

        string token = generateResetToken();
        resetTokens[username] = {token, chrono::system_clock::now() + chrono::seconds(RESET_TOKEN_VALIDITY)};

        recordResetAttempt(username);
        logger.log("Password reset requested for user: " + username);
        return sendResetEmail(email, token);
    }

    // Definition of method to reset a password; takes username, token, and new password as parameters, returns true if password was reset successfully, false otherwise
    bool Auth::resetPassword(const string& username, const string& token, const string& newPassword) {
        auto it = resetTokens.find(username);
        if (it == resetTokens.end() || it->second.first != token) {
            logger.log("Invalid or expired reset token for user: " + username);
            cerr << "Invalid or expired reset token." << endl;
            recordResetAttempt(username);
            return false;
        }

        if (chrono::system_clock::now() > it->second.second) {
            resetTokens.erase(it);
            logger.log("Reset token expired for user: " + username);
            cerr << "Reset token has expired." << endl;
            return false;
        }

        if (!isStrongPassword(newPassword)) {
            logger.log("Password reset failed: New password not strong enough for user: " + username);
            cerr << "New password does not meet strength requirements." << endl;
            return false;
        }

        string hashedPassword = hashPassword(newPassword);
        if (!userDatabase.updateUserPasswordInDatabase(username, hashedPassword)) {
            logger.log("Password reset failed: Database update error for user: " + username);
            cerr << "Failed to update password in database." << endl;
            return false;
        }

        resetTokens.erase(it);
        lastPasswordChange[username] = chrono::system_clock::now();
        logger.log("Password reset successful for user: " + username);
        return true;
    }

    // Definition of method to change a user's password; takes username, old password, and new password as parameters, returns true if password change is successful, false otherwise. Checks for recent password changes, user existence, old password correctness, new password strength, and updates the database. Logs all steps and outcomes.
    bool Auth::changePassword(const string& username, const string& oldPassword, const string& newPassword) {
        if (!canChangePassword(username)) {
            logger.log("Password change rejected: Too soon since last change for user: " + username);
            cerr << "Password was changed recently. Please try again later." << endl;
            return false;
        }

        User user;
        string hashedPassword;
        if (!userDatabase.getUserFromDatabase(username, user, hashedPassword)) {
            logger.log("Password change failed: User not found - " + username);
            cerr << "User not found." << endl;
            return false;
        }

        if (!verifyPassword(oldPassword, hashedPassword)) {
            logger.log("Password change failed: Incorrect old password for user: " + username);
            cerr << "Incorrect old password." << endl;
            return false;
        }

        return updateUserPassword(user, newPassword);
    }

    // Definition of method to recover a user's username; takes email as parameter, returns true if recovery process is successful (username found and email sent), false otherwise. Checks for recent recovery attempts, retrieves username from email, sends recovery email, and updates last recovery timestamp. Logs all steps and outcomes.
    bool Auth :: recoverUsername(const string& email) {
        if (!canRecoverUsername(email)) {
            logger.log("Username recovery rejected: Too soon since last attempt for email: " + email);
            cerr << "Username recovery was requested recently. Please try again later." << endl;
            return false;
        }

        string username;
        if (!userDatabase.getUsernameFromEmail(email, username)) {
            logger.log("Username recovery failed: No account found for email: " + email);
            cerr << "No account found with this email address." << endl;
            return false;
        }

        string subject = "Username Recovery";
        string body = "Your username is: " + username + "\n"
                                                        "If you did not request this information, please contact support immediately.";

        bool result = emailService.sendEmail(email, subject, body);
        if (result) {
            logger.log("Username recovery email sent to: " + email + " for username: " + username);
        } else {
            logger.log("Failed to send username recovery email to: " + email);
        }

        lastUsernameRecovery[email] = chrono::system_clock::now();
        return result;
    }

    // Definition of a method to retrieve a user from a token; takes token as parameter; returns User object
    User Auth::getUserFromToken(const string& token) const {
        try {
            // Log token retrieval attempt (first 10 chars for security)
            cout << "Getting user from token: " << token.substr(0, 10) << "..." << endl;

            // Validate the token
            if (!validateToken(token)) {
                throw runtime_error("Invalid token");
            }

            // Decode the token
            auto decoded = jwt::decode(token);
            cout << "Token decoded successfully" << endl;

            // Log issuer and subject claims
            cout << "Issuer: " << decoded.get_issuer() << endl;
            cout << "Subject: " << decoded.get_subject() << endl;

            // Extract username from token
            string username = decoded.get_subject();
            if (username.empty()) {
                throw runtime_error("Token is missing 'sub' claim");
            }
            cout << "Username extracted from token: " << username << endl;

            // Retrieve user from database
            User user;
            string hashedPassword;
            if (!userDatabase.getUserFromDatabase(username, user, hashedPassword)) {
                throw runtime_error("User not found in database");
            }
            cout << "User found in database" << endl;

            // Log successful retrieval
            logger.log("User retrieved from token successfully: " + username);
            return user;
        } catch (const exception& e) {
            // Log and rethrow exceptions
            cerr << "Error in getUserFromToken: " << e.what() << endl;
            logger.log("Failed to get user from token: " + string(e.what()));
            throw;
        }
    }

}