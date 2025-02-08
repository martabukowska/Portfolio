#pragma once

#include <vector>
#include <map>
#include <string>
#include <optional>
#include <memory>
#include <functional>
#include "user.h"
#include "CSV_management.h"

using namespace std;

namespace TaxReturnSystem {

    // Statistics-related structures
    struct StatsFilter {
        optional<Date> regularDeadline; // Regular deadline filter
        optional<Date> internalDeadline; // Internal deadline filter
        optional<string> manager; // Manager filter
        optional<string> partner; // Partner filter
        optional<string> group; // Group filter
        optional<string> projectType; // Project type filter
    };

    struct ExtensionStats {
        int extended; // Number of extended projects
        int unextended; // Number of unextended projects
        int filed; // Number of filed projects
    };

    struct DeadlineStats {
        int notReviewedByPartner; // Projects not reviewed by partner
        int awaitingReview; // Projects awaiting review
        int awaitingCorrections; // Projects awaiting corrections
        int notFiledYet; // Projects not filed yet
        int filed; // Filed projects
        int filingLate; // Late filing projects
        int awaitingDependencies; // Projects awaiting dependencies
    };

    struct InternalDeadlineStats {
        int total; // Total number of projects
        map<string, int> byGroup; // Projects by group
        map<string, int> byProjectType; // Projects by type
        map<string, int> byPartner; // Projects by partner
        map<string, int> byManager; // Projects by manager
    };

    // Base statistics class
    class Statistics {
    protected:
        shared_ptr<ProjectsDatabase> database; // Database pointer
        map<Date, DeadlineStats> getProjectsPerDeadlineCommon(const StatsFilter& filter) const; // Common deadline stats method

    public:
        Statistics(shared_ptr<ProjectsDatabase> db) : database(db) {} // Constructor
        virtual ~Statistics() = default; // Virtual destructor

        // Pure virtual methods for statistics
        virtual int getTotalProjects(const StatsFilter& filter) const = 0; // Get total projects count
        virtual ExtensionStats getExtensionStats(const StatsFilter& filter) const = 0; // Get extension statistics
        virtual map<Date, DeadlineStats> getProjectsPerDeadline(const StatsFilter& filter) const = 0; // Get deadline statistics
        virtual map<Date, InternalDeadlineStats> getProjectsPerInternalDeadline(const StatsFilter& filter) const = 0; // Get internal deadline statistics

        // Helper methods
        bool projectMatchesFilter(const Project& project, const StatsFilter& filter) const; // Check if project matches filter
        vector<Project> getFilteredProjects(const StatsFilter& filter) const; // Get filtered projects
        virtual unique_ptr<Statistics> clone() const = 0; // Virtual clone method

        vector<Project> getAwaitingCorrectionsProjects(const StatsFilter& filter) const; // Get projects awaiting corrections
        vector<Project> getAwaitingEFileAuthProjects(const StatsFilter& filter) const;   // Get projects awaiting e-file authorization
    };

    // Derived statistics classes
    class BPStatistics : public Statistics {
    public:
        BPStatistics(shared_ptr<ProjectsDatabase> db) : Statistics(db) {} // Constructor

        // Override virtual methods
        int getTotalProjects(const StatsFilter& filter) const override; // Get total projects for BP
        ExtensionStats getExtensionStats(const StatsFilter& filter) const override; // Get extension stats for BP
        map<Date, DeadlineStats> getProjectsPerDeadline(const StatsFilter& filter) const override; // Get deadline stats for BP
        map<Date, InternalDeadlineStats> getProjectsPerInternalDeadline(const StatsFilter& filter) const override; // Get internal deadline stats for BP
        unique_ptr<Statistics> clone() const override { return make_unique<BPStatistics>(*this); } // Clone method
    };

    class PartnerStatistics : public Statistics {
    public:
        PartnerStatistics(shared_ptr<ProjectsDatabase> db) : Statistics(db) {} // Constructor

        // Override virtual methods
        int getTotalProjects(const StatsFilter& filter) const override; // Get total projects for Partner
        ExtensionStats getExtensionStats(const StatsFilter& filter) const override; // Get extension stats for Partner
        map<Date, DeadlineStats> getProjectsPerDeadline(const StatsFilter& filter) const override; // Get deadline stats for Partner
        map<Date, InternalDeadlineStats> getProjectsPerInternalDeadline(const StatsFilter& filter) const override; // Get internal deadline stats for Partner
        unique_ptr<Statistics> clone() const override { return make_unique<PartnerStatistics>(*this); } // Clone method
    };

    class ManagerStatistics : public Statistics {
    public:
        ManagerStatistics(shared_ptr<ProjectsDatabase> db) : Statistics(db) {} // Constructor

        // Override virtual methods
        int getTotalProjects(const StatsFilter& filter) const override; // Get total projects for Manager
        ExtensionStats getExtensionStats(const StatsFilter& filter) const override; // Get extension stats for Manager
        map<Date, DeadlineStats> getProjectsPerDeadline(const StatsFilter& filter) const override; // Get deadline stats for Manager
        map<Date, InternalDeadlineStats> getProjectsPerInternalDeadline(const StatsFilter& filter) const override; // Get internal deadline stats for Manager
        unique_ptr<Statistics> clone() const override { return make_unique<ManagerStatistics>(*this); } // Clone method
    };

    // Statistics factory class
    class StatisticsEngine {
    public:
        static unique_ptr<Statistics> createStatistics(UserRole role, shared_ptr<ProjectsDatabase> database); // Create statistics based on role
    };

} // namespace TaxReturnSystem