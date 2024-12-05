#pragma once

#include <string>
#include <vector>

using namespace std;

namespace TaxReturnSystem {

    // Security settings
    constexpr int MAX_LOGIN_ATTEMPTS = 5; // Maximum allowed login attempts before lockout
    constexpr int LOCKOUT_DURATION = 300; // Duration of lockout in seconds
    constexpr int MAX_RESET_ATTEMPTS = 3; // Maximum password reset attempts
    constexpr int RESET_ATTEMPT_WINDOW = 3600; // Window for reset attempts (1 hour)
    constexpr int PASSWORD_CHANGE_COOLDOWN = 86400; // Cooldown between password changes (24 hours)
    constexpr int USERNAME_RECOVERY_COOLDOWN = 86400; // Cooldown between username recoveries (24 hours)

    // Password and username requirements
    constexpr int MIN_PASSWORD_LENGTH = 8; // Minimum password length
    constexpr int MAX_PASSWORD_LENGTH = 128; // Maximum password length
    constexpr int MIN_USERNAME_LENGTH = 3; // Minimum username length
    constexpr int MAX_USERNAME_LENGTH = 50; // Maximum username length

    // Token settings
    constexpr int RESET_TOKEN_VALIDITY = 3600; // Reset token validity period (1 hour)
    constexpr int TOKEN_EXPIRY = 3600; // JWT token expiry period (1 hour)
    constexpr int RESET_TOKEN_LENGTH = 32; // Length of reset tokens
    constexpr const char *TOKEN_CHARS = "23456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnpqrstuvwxyz"; // Valid characters for tokens

    // JWT configuration
    constexpr const char *JWT_ISSUER = "auth_service"; // JWT issuer identifier

    // CSV column configuration
    static constexpr int EXPECTED_COLUMN_COUNT = 9; // Expected number of columns in CSV

    // CSV column indices
    extern int DUE_DATE_COLUMN; // Column index for due date
    extern int NEXT_TASK_COLUMN; // Column index for next task
    extern int MANAGER_COLUMN; // Column index for manager
    extern int PARTNER_COLUMN; // Column index for partner
    extern int CLIENT_COLUMN; // Column index for client
    extern int GROUP_COLUMN; // Column index for group
    extern int PROJECT_COLUMN; // Column index for project
    extern int BILLING_PARTNER_COLUMN; // Column index for billing partner
    extern int MEMO_COLUMN; // Column index for memo

    // Filter option constants
    const int FILTER_BY_MANAGER = 1; // Filter by manager option
    const int FILTER_BY_PARTNER = 2; // Filter by partner option
    const int FILTER_BY_BILLING_PARTNER = 3; // Filter by billing partner option
    const int FILTER_BY_GROUP = 4; // Filter by group option
    const int FILTER_BY_PROJECT = 5; // Filter by project option
    const int FILTER_BY_DUE_DATE = 6; // Filter by due date option
    const int FILTER_BY_NEXT_TASK = 7; // Filter by next task option

    // Task status constants
    const string TASK_BILLED = "Billed"; // Status for billed tasks
    const string TASK_TAX_RETURN_FILED = "Tax Return Filed"; // Status for filed returns
    const string TASK_EFILE_SENT = "E-file Sent to Client"; // Status for sent e-files
    const string TASK_EFILE_SIGNED = "E-file Signed by Client"; // Status for signed e-files
    const string TASK_CORRECTIONS_CLEARED = "Corrections Cleared"; // Status for cleared corrections
    const string TASK_PARTNER_REVIEWED = "Partner Reviewed"; // Status for partner review

    // System configuration
    const vector<string> billingPartners = {"MOTI", "EFROIM", "JEFF", "JACOB"}; // List of billing partners
    const string FILE_NAME = "Book1.csv"; //TODO: adjust later on

} // namespace TaxReturnSystem