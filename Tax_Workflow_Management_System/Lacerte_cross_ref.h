#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <dlib/svm.h>
#include <chrono>
#include "AD_CSV_management.h"

using namespace std;

namespace TaxReturnSystem {

    // Stores match results between Lacerte and database systems
    struct MatchResult {
        string lacerteName;      // Name from Lacerte system
        string databaseMatch;    // Matching name from database
        double confidence;       // Confidence score of the match
    };

    // Handles cross-referencing and matching between Lacerte and database systems
    class LacerteCrossReference {
    private:
        ProjectsDatabase& database;      // Reference to projects database
        map<string, vector<string>> equivalentTerms;    // Map of equivalent terms

        // String processing methods
        string preprocessName(const string& name);      // Clean and standardize input name
        vector<string> tokenizeAndSort(const string& name); // Split name into sorted tokens
        double tokenOverlap(const string& name1, const string& name2); // Calculate name overlap

        void initializeEquivalentTerms();    // Initialize equivalent terms mapping
        bool areTokensEquivalent(const string& token1, const string& token2); // Check token equivalence
        int calculateLevenshteinDistance(const string& s1, const string& s2); // Calculate edit distance

        dec_funct_type matcher_model;    // SVM model for matching

        // Stores training data pairs
        struct TrainingPair {
            string system1_name;     // Name from first system
            string system2_name;     // Name from second system
            bool is_match;          // Whether names match
        };
        vector<TrainingPair> training_data;    // Storage for training pairs
        vector<vector<string>> readCSV(const string& filename); // Read training data from CSV

        // Parameters for online learning
        struct OnlineLearningParams {
            int minMismatchesForRetrain = 50;     // Minimum mismatches before retraining
            int hoursBeforeRetrain = 24;          // Hours between retraining
            int maxRecentMismatches = 100;        // Maximum stored mismatches
            double retrainAccuracyThreshold = 0.95; // Accuracy threshold for retraining
        } learningParams;

    public:
        // Type definitions for SVM model implementation
        using sample_type = dlib::matrix<double,0,1>;      // Matrix type for feature vectors
        using kernel_type = dlib::linear_kernel<sample_type>; // Kernel type for SVM
        using dec_funct_type = dlib::decision_function<kernel_type>; // Decision function type

        // Stores precomputed feature vectors for database entries
        struct PrecomputedFeatures {
            string clientName;   // Name of the client
            sample_type features; // Precomputed feature vector
        };

        LacerteCrossReference(ProjectsDatabase& db) : database(db) {} // Initialize with database reference
        void loadTrainingData(const string& filename);    // Load training data from CSV file
        void trainModel();                               // Train the SVM model
        double getMatchConfidence(const string& name1, const string& name2); // Get match confidence score

        // Feature computation methods
        sample_type nameToFeatures(const string& name);   // Convert name to feature vector
        sample_type concatenateFeatures(const sample_type& f1, const sample_type& f2); // Combine feature vectors

        // Database feature pre-computation
        vector<PrecomputedFeatures> precomputeDatabaseFeatures(const vector<Project>& projects); // Precompute all database features

        // Model feedback and learning
        void updateModelWithFeedback(
                const string& name1,           // First name to compare
                const string& name2,           // Second name to compare
                bool isCorrectMatch,           // Whether names actually match
                double predictedConfidence,    // Model's confidence score
                bool forceRetrain = false);    // Force model retraining

        // Stores metrics about model performance
        struct ModelMetrics {
            double accuracy;     // Overall model accuracy
            int totalPredictions; // Total predictions made
            vector<pair<string,string>> recentMismatches; // Recent incorrect matches
            chrono::system_clock::time_point lastUpdate;  // Time of last update

            int matches_found = 0;    // Total matches found
            int correct_matches = 0;   // Number of correct matches
            int total_high_confidence = 0; // High confidence predictions
            double batch_accuracy = 0.0;   // Current batch accuracy

            ModelMetrics() : accuracy(0.0), totalPredictions(0),  // Initialize metrics
                           lastUpdate(chrono::system_clock::now()) {}

            double getAccuracyRate() const { return totalPredictions > 0 ? accuracy : 0.0;} // Get current accuracy rate
            double getMatchRate() const { return totalPredictions > 0 ? static_cast<double>(matches_found) / totalPredictions : 0.0;} // Get current match rate
        };

        ModelMetrics metrics;                    // Current model metrics
        ModelMetrics getModelMetrics() const;    // Get copy of current metrics

        // Stores indexed features for optimized matching
        struct IndexedFeatures {
            vector<pair<string, sample_type>> features;    // Stored feature vectors
            map<int, vector<int>> wordCountIndex;          // Word count to indices mapping
        };

        IndexedFeatures featureIndex;    // Indexed features for faster matching
    };

} // namespace TaxReturnSystem