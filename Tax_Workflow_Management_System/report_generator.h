#pragma once

#include "CSV_management.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <chrono>
#include <map>

using namespace std;

namespace TaxReturnSystem {

    // Class for handling project condition checks
    class ReportConditions {
    private:
        static bool isExtended(const string &billingPartner); // Check if project is extended based on billing partner

    public:
        // Project status checking methods
        static bool isNotFiled(const Project &project); // Check if project is not filed
        static bool isNotReviewed(const Project &project); // Check if project is not reviewed
        static bool isAwaitingCorrections(const Project &project); // Check if project needs corrections
        static bool isAwaitingEFileAuthorization(const Project &project); // Check if project awaits e-file auth
        static bool isUnextended(const Project &project); // Check if project is unextended
        static bool isInDeadline(const Project &project, const Date &deadline); // Check if project is in specific deadline
    };

    // Class for generating various reports
    class ReportGenerator {
    private:
        static void writeCSV(const string &filename, const vector<std::vector<std::string>> &data); // Write data to CSV file

    public:
        // Report generation methods
        static void generateReport(const ProjectsDatabase &database, const string &filename,
                                   const function<bool(const Project &)> &condition); // Generate conditional report

        static void generateDeadlineReport(const ProjectsDatabase &database, const Date &deadline,
                                           const string &filename); // Generate deadline-specific report
    };

} // namespace TaxReturnSystem