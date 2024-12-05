/**
 * @file report_generator.cpp
 * @brief Implementation of report generation functionality for the Tax Return System
 *
 * This file contains implementations for:
 * - Checking project conditions (e.g., filed, reviewed, extended)
 * - Writing data to CSV files
 * - Generating reports based on specific conditions
 * - Managing deadlines for report generation
 *
 * The ReportGenerator class provides methods to create reports
 * based on project status and deadlines, exporting them to CSV format.
 */

#include "report_generator.h"
#include <chrono>
#include <regex>
#include <unordered_set>
#include <stdexcept>

using namespace std;

namespace TaxReturnSystem {

    // Task sets for different report types
    const unordered_set<string> TASKS_FILED = {
            "Billed", "Tax Return Filed", "E-file Sent to Client", "E-file Signed by Client", "Corrections Cleared"
    };

    const unordered_set<string> TASKS_PARTNER_FILED = {
            "Billed", "Tax Return Filed", "E-file Sent to Client", "E-file Signed by Client", "Corrections Cleared",
            "Partner Reviewed"
    };

    // Method for checking if a project is extended; takes billing partner string as parameter; returns bool
    bool ReportConditions::isExtended(const string &billingPartner) {
        return billingPartner.find("Extended") != string::npos;
    }

    // Method for checking if a project is not filed; takes Project as parameter; returns bool
    bool ReportConditions::isNotFiled(const Project &project) {
        return TASKS_FILED.find(project.getNextTask()) == TASKS_FILED.end();
    }

    // Method for checking if a project is not reviewed; takes Project as parameter; returns bool
    bool ReportConditions::isNotReviewed(const Project &project) {
        return TASKS_PARTNER_FILED.find(project.getNextTask()) == TASKS_PARTNER_FILED.end();
    }

    // Method for checking if a project is awaiting corrections; takes Project as parameter; returns bool
    bool ReportConditions::isAwaitingCorrections(const Project &project) {
        return project.getNextTask() == "Corrections Cleared";
    }

    // Method for checking if a project is awaiting e-file authorization; takes Project as parameter; returns bool
    bool ReportConditions::isAwaitingEFileAuthorization(const Project &project) {
        return project.getNextTask() == "E-file Signed by Client";
    }

    // Method for checking if a project is unextended; takes Project as parameter; returns bool
    bool ReportConditions::isUnextended(const Project &project) {
        return !isExtended(project.getBillingPartner());
    }

    // Method for checking if a project is in a specific deadline; takes Project and Date as parameters; returns bool
    bool ReportConditions::isInDeadline(const Project &project, const Date &deadline) {
        return project.isInDeadline(deadline, ReportType::RegularDeadline) ||
               project.isInDeadline(deadline, ReportType::InternalDeadline);
    }

    // Method for writing data to a CSV file; takes filename and vector of strings as parameters; returns void
    void ReportGenerator::writeCSV(const string &filename, const vector<vector<string>> &data) {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            throw runtime_error("Error: Could not open file for writing: " + filename);
        }

        for (const auto &row : data) {
            for (size_t i = 0; i < row.size(); ++i) {
                outFile << row[i];
                if (i < row.size() - 1) outFile << ",";
            }
            outFile << "\n";
        }

        outFile.close();
    }

    // Method for generating a report based on a condition; takes ProjectsDatabase, filename, and condition function as parameters; returns void
    void ReportGenerator::generateReport(const ProjectsDatabase &database, const string &filename,
                                         const function<bool(const Project &)> &condition) {
        vector<vector<string>> reportData;
        reportData.push_back({"Project", "Client", "Manager", "Due Date", "Memo", "Next Task"});

        for (const auto &project : database.getAllProjects()) {
            if (condition(project)) {
                reportData.push_back({
                                             project.getProjectType(),
                                             project.getClient(),
                                             project.getManager(),
                                             project.getRegularDeadline().getDateStr(),
                                             project.getMemo(),
                                             project.getNextTask()
                                     });
            }
        }

        writeCSV(filename, reportData);
        cout << "Report exported to " << filename << endl;
    }

    // Method for generating a deadline-specific report; takes ProjectsDatabase, Date, and filename as parameters; returns void
    void ReportGenerator::generateDeadlineReport(const ProjectsDatabase &database, const Date &deadline, const string &filename) {
        generateReport(database, filename, [deadline](const Project &project) {
            return ReportConditions::isInDeadline(project, deadline);
        });
        cout << "Deadline Report for " << deadline.getDateStr() << " exported to " << filename << endl;
        cout << "Note: Remember to add remaining projects to these deadlines." << endl;
    }

} // namespace TaxReturnSystem