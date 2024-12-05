/**
 * @file statistics.cpp
 * @brief Implementation of statistics-related functionality for the Tax Return System
 *
 * This file implements various statistics calculations and filtering methods
 * for different user roles, including:
 * - Project filtering based on criteria
 * - Deadline and extension statistics
 * - Role-specific statistics calculations
 */

#include "statistics.h"
#include "CSV_management.h"

namespace TaxReturnSystem {

    // Definition of a method to check if a project matches filter criteria; takes Project and StatsFilter as parameters; returns bool
    bool Statistics::projectMatchesFilter(const Project& project, const StatsFilter& filter) const {
        // Check if project matches regular deadline filter
        if (filter.regularDeadline) {
            if (project.getReportType() == ReportType::RegularDeadline) {
                if (project.getRegularDeadline() > *filter.regularDeadline) return false;
            } else {
                // Exclude projects without regular deadline when filter requires one
                return false;
            }
        }

        // Check if project matches internal deadline filter
        if (filter.internalDeadline) {
            if (project.getReportType() == ReportType::InternalDeadline) {
                if (project.getInternalDeadline() > *filter.internalDeadline) return false;
            } else {
                // Exclude projects without internal deadline when filter requires one
                return false;
            }
        }

        // Check if project matches other filter criteria
        if (filter.manager && project.getManager() != *filter.manager) return false;
        if (filter.partner && project.getPartner() != *filter.partner) return false;
        if (filter.group && project.getGroup() != *filter.group) return false;
        if (filter.projectType && project.getProjectType() != *filter.projectType) return false;

        // Project matches all filter criteria
        return true;
    }

    // Definition of a method to get filtered projects; takes StatsFilter as parameter; returns vector of Projects
    vector<Project> Statistics::getFilteredProjects(const StatsFilter& filter) const {
        // Initialize vector for filtered projects
        vector<Project> filteredProjects;

        // Iterate through all projects and apply filter
        for (const auto& project : database->getAllProjects()) {
            if (projectMatchesFilter(project, filter)) {
                filteredProjects.push_back(project);
            }
        }

        return filteredProjects;
    }

    // Definition of a method to get statistics per deadline; takes StatsFilter as parameter; returns map of Date to DeadlineStats
    map<Date, DeadlineStats> Statistics::getProjectsPerDeadlineCommon(const StatsFilter& filter) const {
        // Initialize statistics map
        map<Date, DeadlineStats> stats;

        // Process each filtered project
        for (const auto& project : getFilteredProjects(filter)) {
            // Determine appropriate deadline based on report type
            Date deadline;
            if (project.getReportType() == ReportType::RegularDeadline) {
                deadline = project.getRegularDeadline();
            } else {
                deadline = project.getInternalDeadline();
            }

            // Skip projects with empty deadlines
            if (deadline.getDateStr().empty()) {
                continue;
            }

            // Get or create stats for this deadline
            DeadlineStats& deadlineStats = stats[deadline];

            // Check project dependencies
            bool dependenciesMet = true;
            for (const auto& [dependencyId, dependencyType] : project.getDependencies()) {
                Project dependencyProject;
                if (database->getProjectFromDatabase(dependencyId, dependencyProject)) {
                    if (!project.isDependencyMet(dependencyId, dependencyProject.getNextTask())) {
                        dependenciesMet = false;
                        break;
                    }
                } else {
                    // Skip if dependency project not found
                    dependenciesMet = false;
                    break;
                }
            }

            // Handle projects with unmet dependencies
            if (!dependenciesMet) {
                deadlineStats.awaitingDependencies++;
                continue;
            }

            // Get current task and update relevant statistics
            string nextTask = project.getNextTask();

            // Count projects not reviewed by partner
            if (nextTask == "Signed Engagement Letter" ||
                nextTask == "Sent Open Items - Extension" ||
                nextTask == "Information Entered" ||
                nextTask == "Sent Open Items - Final Preparation" ||
                nextTask == "Final Information Entered" ||
                nextTask == "Ready for Manager Review" ||
                nextTask == "Manager Approved - Ready for Partner Review") {
                deadlineStats.notReviewedByPartner++;
            }
                // Count projects awaiting review
            else if (nextTask == "Partner Reviewed") {
                deadlineStats.awaitingReview++;
            }
                // Count projects awaiting corrections
            else if (nextTask == "Corrections Cleared") {
                deadlineStats.awaitingCorrections++;
            }
                // Count projects not filed yet
            else if (nextTask == "Corrections Cleared" ||
                     nextTask == "E-file Sent to Client" ||
                     nextTask == "E-file Signed by Client" ||
                     nextTask == "Tax Return Filed") {
                deadlineStats.notFiledYet++;
            }
                // Count filed projects
            else if (nextTask == "Billed") {
                deadlineStats.filed++;
            }

            // Check filing late status based on extension and billing partner
            if (project.isExtended() || project.getBillingPartner().find("Filing Late") != string::npos) {
                if (project.isExtended()) {
                    deadlineStats.filingLate++;
                }
                if (project.getBillingPartner().find("Filing Late") != string::npos) {
                    deadlineStats.filingLate++;
                }
            }
        }

        return stats;
    }

    // BILLING PARTNER STATISTICS IMPLEMENTATION

    // Definition of a method to get the total number of filtered projects; takes StatsFilter as parameter; returns int
    int BPStatistics::getTotalProjects(const StatsFilter& filter) const {
        // Get the size of the filtered projects vector
        return getFilteredProjects(filter).size();
    }

    // Definition of a method to get extension statistics; takes StatsFilter as parameter; returns ExtensionStats
    ExtensionStats BPStatistics::getExtensionStats(const StatsFilter& filter) const {
        // Initialize stats structure with zeros for extended, filed, and unextended counts
        ExtensionStats stats = {0, 0, 0};

        // Process each filtered project
        for (const auto& project : getFilteredProjects(filter)) {
            // Check if project is extended (either by flag or billing partner)
            if (project.isExtended() || project.getBillingPartner().find("Extended") != string::npos) {
                if (project.isExtended()) {
                    // Increment for extended flag
                    stats.extended++;
                }
                if (project.getBillingPartner().find("Extended") != string::npos) {
                    // Increment for extended billing partner
                    stats.extended++;
                }
            }
                // Check if project is filed
            else if (project.getNextTask() == "Billed") {
                stats.filed++;
            }
                // Project is neither extended nor filed
            else {
                stats.unextended++;
            }
        }

        return stats;
    }

    // Definition of a method to get deadline statistics; takes StatsFilter as parameter; returns map of Date to DeadlineStats
    map<Date, DeadlineStats> BPStatistics::getProjectsPerDeadline(const StatsFilter& filter) const {
        // Delegate to common implementation in base class
        return getProjectsPerDeadlineCommon(filter);
    }

    // Definition of a method to get statistics per internal deadline; takes StatsFilter as parameter; returns map of Date to InternalDeadlineStats
    map<Date, InternalDeadlineStats> BPStatistics::getProjectsPerInternalDeadline(const StatsFilter& filter) const {
        // Initialize statistics map
        map<Date, InternalDeadlineStats> stats;

        // Process each filtered project
        for (const auto& project : getFilteredProjects(filter)) {
            // Determine appropriate deadline based on report type
            Date deadline = (project.getReportType() == ReportType::InternalDeadline)
                            ? project.getInternalDeadline()
                            : project.getRegularDeadline();

            // Get or create stats for this deadline
            auto& internalDeadlineStats = stats[deadline];

            // Update statistics for this project
            internalDeadlineStats.total++;
            internalDeadlineStats.byGroup[project.getGroup()]++;
            internalDeadlineStats.byProjectType[project.getProjectType()]++;
            internalDeadlineStats.byPartner[project.getPartner()]++;
            internalDeadlineStats.byManager[project.getManager()]++;
        }

        return stats;
    }

    // PARTNER STATISTICS IMPLEMENTATION

    // Definition of a method to get the total number of filtered projects; takes StatsFilter as parameter; returns int
    int PartnerStatistics::getTotalProjects(const StatsFilter& filter) const {
        // Get the size of the filtered projects vector
        return getFilteredProjects(filter).size();
    }

    // Definition of a method to get extension statistics; takes StatsFilter as parameter; returns ExtensionStats
    ExtensionStats PartnerStatistics::getExtensionStats(const StatsFilter& filter) const {
        // Initialize stats structure with zeros for extended, filed, and unextended counts
        ExtensionStats stats = {0, 0, 0};

        // Process each filtered project
        for (const auto& project : getFilteredProjects(filter)) {
            // Check if project is extended (either by flag or billing partner)
            if (project.isExtended() || project.getBillingPartner().find("Extended") != string::npos) {
                if (project.isExtended()) {
                    // Increment for extended flag
                    stats.extended++;
                }
                if (project.getBillingPartner().find("Extended") != string::npos) {
                    // Increment for extended billing partner
                    stats.extended++;
                }
            }
                // Check if project is filed
            else if (project.getNextTask() == "Billed") {
                stats.filed++;
            }
                // Project is neither extended nor filed
            else {
                stats.unextended++;
            }
        }

        return stats;
    }

    // Definition of a method to get deadline statistics; takes StatsFilter as parameter; returns map of Date to DeadlineStats
    map<Date, DeadlineStats> PartnerStatistics::getProjectsPerDeadline(const StatsFilter& filter) const {
        // Delegate to common implementation in base class
        return getProjectsPerDeadlineCommon(filter);
    }

    // Definition of a method to get statistics per internal deadline; takes StatsFilter as parameter; returns map of Date to InternalDeadlineStats
    map<Date, InternalDeadlineStats> PartnerStatistics::getProjectsPerInternalDeadline(const StatsFilter& filter) const {
        // Initialize statistics map
        map<Date, InternalDeadlineStats> stats;

        // Process each filtered project
        for (const auto& project : getFilteredProjects(filter)) {
            // Determine appropriate deadline based on report type
            Date deadline = (project.getReportType() == ReportType::InternalDeadline)
                            ? project.getInternalDeadline()
                            : project.getRegularDeadline();

            // Get or create stats for this deadline
            auto& internalDeadlineStats = stats[deadline];

            // Update statistics for this project
            internalDeadlineStats.total++;
            internalDeadlineStats.byGroup[project.getGroup()]++;
            internalDeadlineStats.byProjectType[project.getProjectType()]++;
            internalDeadlineStats.byManager[project.getManager()]++;
        }

        return stats;
    }

    // MANAGER STATISTICS IMPLEMENTATION

    // Definition of a method to get the total number of filtered projects; takes StatsFilter as parameter; returns int
    int ManagerStatistics::getTotalProjects(const StatsFilter& filter) const {
        // Get the size of the filtered projects vector
        return getFilteredProjects(filter).size();
    }

    // Definition of a method to get extension statistics; takes StatsFilter as parameter; returns ExtensionStats
    ExtensionStats ManagerStatistics::getExtensionStats(const StatsFilter& filter) const {
        // Initialize stats structure with zeros for extended, filed, and unextended counts
        ExtensionStats stats = {0, 0, 0};

        // Process each filtered project
        for (const auto& project : getFilteredProjects(filter)) {
            // Check if project is extended (either by flag or billing partner)
            if (project.isExtended() || project.getBillingPartner().find("Extended") != string::npos) {
                if (project.isExtended()) {
                    // Increment for extended flag
                    stats.extended++;
                }
                if (project.getBillingPartner().find("Extended") != string::npos) {
                    // Increment for extended billing partner
                    stats.extended++;
                }
            }
                // Check if project is filed
            else if (project.getNextTask() == "Billed") {
                stats.filed++;
            }
                // Project is neither extended nor filed
            else {
                stats.unextended++;
            }
        }

        return stats;
    }

    // Definition of a method to get deadline statistics; takes StatsFilter as parameter; returns map of Date to DeadlineStats
    map<Date, DeadlineStats> ManagerStatistics::getProjectsPerDeadline(const StatsFilter& filter) const {
        // Delegate to common implementation in base class
        return getProjectsPerDeadlineCommon(filter);
    }

    // Definition of a method to get statistics per internal deadline; takes StatsFilter as parameter; returns map of Date to InternalDeadlineStats
    map<Date, InternalDeadlineStats> ManagerStatistics::getProjectsPerInternalDeadline(const StatsFilter& filter) const {
        // Initialize statistics map
        map<Date, InternalDeadlineStats> stats;

        // Process each filtered project
        for (const auto& project : getFilteredProjects(filter)) {
            // Determine appropriate deadline based on report type
            Date deadline = (project.getReportType() == ReportType::InternalDeadline)
                            ? project.getInternalDeadline()
                            : project.getRegularDeadline();

            // Get or create stats for this deadline
            auto& internalDeadlineStats = stats[deadline];

            // Update statistics for this project
            internalDeadlineStats.total++;
            internalDeadlineStats.byGroup[project.getGroup()]++;
            internalDeadlineStats.byProjectType[project.getProjectType()]++;
            internalDeadlineStats.byPartner[project.getPartner()]++;
        }

        return stats;
    }

    // Definition of a factory method to create statistics object based on user role; takes UserRole and ProjectsDatabase; returns unique_ptr to Statistics
    unique_ptr<Statistics> StatisticsEngine::createStatistics(UserRole role, shared_ptr<ProjectsDatabase> database) {
        // Create appropriate statistics object based on user role
        switch (role) {
            case UserRole::BillingPartner:
                return make_unique<BPStatistics>(database);
            case UserRole::Partner:
                return make_unique<PartnerStatistics>(database);
            case UserRole::Manager:
                return make_unique<ManagerStatistics>(database);
            default:
                // Throw error for unsupported roles
                throw runtime_error("Invalid user role");
        }
    }

}