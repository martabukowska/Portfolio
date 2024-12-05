/**
 * @file config.cpp
 * @brief Implementation file for managing CSV column indices in the Tax Return System
 *
 * This file contains the definitions for CSV column indices used in the system.
 * These indices are initialized to -1 and will be set when reading the CSV header.
 *
 * The indices include:
 * - Memo
 * - Group
 * - Client
 * - Manager
 * - Partner
 * - Project
 * - Due Date
 * - Next Task
 * - Billing Partner
 */

#include "config.h"

namespace TaxReturnSystem {

    // CSV column indices for data fields
    // All indices are initialized to -1 to indicate unset state
    // These values will be updated when reading the CSV header

    int MEMO_COLUMN = -1;            // Index for memo/notes column
    int GROUP_COLUMN = -1;           // Index for group column
    int CLIENT_COLUMN = -1;          // Index for client name column
    int MANAGER_COLUMN = -1;         // Index for project manager column
    int PARTNER_COLUMN = -1;         // Index for partner column
    int PROJECT_COLUMN = -1;         // Index for project type column
    int DUE_DATE_COLUMN = -1;        // Index for project due date column (internal or external, depending on the report type)
    int NEXT_TASK_COLUMN = -1;       // Index for next task/status column
    int BILLING_PARTNER_COLUMN = -1; // Index for billing partner column

} // namespace TaxReturnSystem