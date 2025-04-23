#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <dlib/svm.h>
#include <chrono>
#include "CSV_management.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>

using namespace std;

namespace TaxReturnSystem {

    // Struct for storing match results between two systems
    struct MatchResult {
        std::string lacerteName; // Name from Lacerte system
        std::string databaseMatch; // Matching name from database
        double confidence; // Confidence score of the match
    };

    class LacerteCrossReference {
    public:
        // Type definitions for SVM implementation
        using sample_type = dlib::matrix<double,0,1>; // Matrix type for feature vectors
        using kernel_type = dlib::linear_kernel<sample_type>; // Kernel type for SVM
        using dec_funct_type = dlib::decision_function<kernel_type>; // Decision function type

        // Structure for storing precomputed features
        struct PrecomputedFeatures {
            string clientName;
            string processedName;
            vector<string> tokens;
            sample_type features;
            unordered_set<string> tokenSet;  // For faster lookups
        };

        // Constructor and main methods
        LacerteCrossReference(ProjectsDatabase& db) : database(db) {} // Initialize the cross reference system
        void loadTrainingData(const string& filename); // Load training data from file
        void trainModel(); // Train the SVM model
        double getMatchConfidence(const string& name1, const string& name2); // Confidence score calculation
        double getMatchConfidence(const PrecomputedFeatures& features1, const PrecomputedFeatures& features2); // Confidence score calculation
        // Feature computation and model methods
        sample_type nameToFeatures(const string& name); // Convert name to feature vector
        sample_type concatenateFeatures(const sample_type& f1, const sample_type& f2); // Combine two feature vectors

        // Pre-computation methods
        vector<PrecomputedFeatures> precomputeDatabaseFeatures(const vector<Project>& projects); // Precompute features for database entries

        // Online learning methods
        void updateModelWithFeedback(
                const string& name1, const string& name2,
                bool isCorrectMatch, double predictedConfidence,
                bool forceRetrain = false);

        // Metrics structure for tracking model performance
        struct ModelMetrics {
            double accuracy; // Overall model accuracy
            int totalPredictions; // Total number of predictions made
            vector<pair<string,string>> recentMismatches; // Recent incorrect matches
            chrono::system_clock::time_point lastUpdate; // Time of last model update

            int matchesFound = 0; // Total matches found
            int correctMatches = 0; // Number of correct matches
            int totalHighConfidence = 0; // Total number of high confidence predictions

            ModelMetrics() : accuracy(0.0), totalPredictions(0), // Constructor with default values
                             lastUpdate(chrono::system_clock::now()) {}

            double getAccuracyRate() const { // Get current accuracy rate
                return totalPredictions > 0 ? accuracy : 0.0;
            }

            double getMatchRate() const { // Get current match rate
                return totalPredictions > 0 ?
                       static_cast<double>(matchesFound) / totalPredictions : 0.0;
            }
        };

        ModelMetrics metrics; // Current model metrics
        ModelMetrics getModelMetrics() const; // Get copy of current metrics

        // Structure for indexed features to optimize matching
        struct IndexedFeatures {
            vector<pair<string, sample_type>> features; // Stored feature vectors
            map<int, vector<int>> wordCountIndex; // Word count to indices mapping
        };

        IndexedFeatures featureIndex; // Indexed features for faster matching

        // Add findMatches as a class method
        vector<MatchResult> findMatches(const vector<string>& lacerteNames,
                                        const vector<PrecomputedFeatures>& precomputed);

        bool testDatabaseAccess(); // Tests database connectivity by attempting to store test feedback data

        void benchmarkPrecompute(const vector<Project>& projects); // Measures and reports precomputation performance for a set of projects

    private:
        ProjectsDatabase& database;  // Reference to the database

        map<string, vector<string>> equivalentTerms; // Map of equivalent terms for matching

        // String processing methods
        string preprocessName(const string& name); // Clean and standardize input name
        vector<string> tokenizeAndSort(const string& name); // Split name into sorted tokens
        double tokenOverlap(const PrecomputedFeatures& features1,
                            const PrecomputedFeatures& features2); // Calculate overlap between names

        void initializeEquivalentTerms(); // Initialize map of equivalent terms
        bool areTokensEquivalent(const string& token1, const string& token2); // Check if tokens are equivalent
        int calculateLevenshteinDistance(const string& s1, const string& s2); // Calculate edit distance

        // AI model components
        dlib::decision_function<kernel_type> matcher_model; // SVM model for matching

        // Training data structure and storage
        struct TrainingPair {
            string system1_name; // Name from first system
            string system2_name; // Name from second system
            bool is_match; // Whether names match
        };
        vector<TrainingPair> training_data; // Storage for training pairs
        vector<vector<string>> readCSV(const string& filename); // Read training data from CSV

        // Online learning parameters
        struct OnlineLearningParams {
            int minMismatchesForRetrain = 50; // Minimum mismatches before retraining
            int hoursBeforeRetrain = 24; // Hours between retraining
            int maxRecentMismatches = 100; // Maximum stored mismatches
            double retrainAccuracyThreshold = 0.95; // Accuracy threshold for retraining
        } learningParams;
    };

} // namespace TaxReturnSystem