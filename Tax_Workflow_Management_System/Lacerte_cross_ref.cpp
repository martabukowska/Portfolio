/**
 * @file Lacerte_cross_ref.cpp
 * @brief Implementation of the LacerteCrossReference class for the Tax Return System
 *
 * This file provides the implementation of the LacerteCrossReference class, which includes
 * methods for cross-referencing and matching company names between different tax systems,
 * training the matching model, and handling user feedback for continuous learning.
 */

#include "Lacerte_cross_ref.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>
#include "config.h"

using namespace std;

namespace TaxReturnSystem {

    // Definition of method to initialize equivalent terms mapping; takes no parameters; returns void
    void LacerteCrossReference::initializeEquivalentTerms() {
        equivalentTerms = {
                {"&", {"and"}},
                {"intl", {"international"}},
                {"corp", {"corporation"}},
                {"inc", {"incorporated"}},
                {"llc", {"limited", "liability", "company"}},
                {"ltd", {"limited"}},
                {"svcs", {"services"}},
                {"svc", {"service"}},
                {"mgt", {"management"}},
                {"mgmt", {"management"}},
                {"ave", {"avenue"}},
                {"st", {"street"}},
                {"E", {"East"}},
                {"@", {"at"}},
                {"opco", {"operating"}},
                {"holdco", {"holding"}},
                {"holdco", {"holdings"}},
                {"tom", {"thomas"}},
                {"dan", {"daniel"}},
                {"dave", {"david"}}
        };
    }

    // Definition of method to read CSV file; takes filename string parameter; returns vector of string vectors
    vector<vector<string>> LacerteCrossReference::readCSV(const string& filename) {
        vector<vector<string>> data;
        ifstream file(filename);
        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            string item;
            vector<string> row;

            while (getline(ss, item, ',')) {
                row.push_back(item);
            }
            data.push_back(row);
        }
        return data;
    }

    // Definition of method to preprocess a name; takes name string parameter; returns processed string
    string LacerteCrossReference::preprocessName(const string& name) {
        string processed = name;

        // Business abbreviations to normalize
        const vector<pair<string, string>> businessAbbreviations = {
                {"llc", ""},
                {"ltd", ""},
                {"inc", ""},
                {"corp", ""},
                {"corporation", ""},
                {"incorporated", ""},
                {"limited", ""},
                {"company", ""},
                {"co", ""},
                {"lp", ""},
                {"llp", ""},
                {"l.l.c.", ""},
                {"l.t.d.", ""},
                {"l.p.", ""},
                {"l.l.p.", ""}
        };

        // 1. Convert to lowercase
        transform(processed.begin(), processed.end(), processed.begin(), ::tolower);

        // Store original business suffix for later comparison
        string originalSuffix;
        for (const auto& abbr : businessAbbreviations) {
            size_t pos = processed.find(abbr.first);
            if (pos != string::npos) {
                originalSuffix = processed.substr(pos);
                break;
            }
        }

        // 2. Remove business abbreviations for main comparison
        for (const auto& abbr : businessAbbreviations) {
            size_t pos;
            while ((pos = processed.find(abbr.first)) != string::npos) {
                processed.replace(pos, abbr.first.length(), "");
            }
        }

        // 3. Replace special characters with their word equivalents
        for (const auto& term : equivalentTerms) {
            size_t pos;
            while ((pos = processed.find(term.first)) != string::npos) {
                processed.replace(pos, term.first.length(), term.second[0]);
            }
        }

        // 4. Remove remaining punctuation and special characters
        processed = regex_replace(processed, regex("[.,;:'\"\\-_]"), "");

        // 5. Normalize spaces (multiple spaces to single space)
        processed = regex_replace(processed, regex("\\s+"), " ");

        // 6. Trim leading/trailing spaces
        processed = regex_replace(processed, regex("^\\s+|\\s+$"), "");

        return processed;
    }

    // Definition of method to tokenize and sort a name; takes name string parameter; returns vector of tokens
    vector<string> LacerteCrossReference::tokenizeAndSort(const string& name) {
        vector<string> tokens;
        stringstream ss(name);
        string token;

        while (ss >> token) {
            tokens.push_back(token);
        }
        sort(tokens.begin(), tokens.end());
        return tokens;
    }

    // Definition of method to calculate Levenshtein distance; takes two strings as parameters; returns distance as integer
    int LacerteCrossReference::calculateLevenshteinDistance(const string& s1, const string& s2) {
        vector<vector<int>> dp(s1.length() + 1, vector<int>(s2.length() + 1));

        for (int i = 0; i <= s1.length(); i++)
            dp[i][0] = i;
        for (int j = 0; j <= s2.length(); j++)
            dp[0][j] = j;

        for (int i = 1; i <= s1.length(); i++) {
            for (int j = 1; j <= s2.length(); j++) {
                if (s1[i-1] == s2[j-1])
                    dp[i][j] = dp[i-1][j-1];
                else
                    dp[i][j] = 1 + min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
            }
        }

        return dp[s1.length()][s2.length()];
    }

    // Definition of method to check token equivalence; takes two token strings as parameters; returns boolean
    bool LacerteCrossReference::areTokensEquivalent(const string& token1, const string& token2) {
        if (token1 == token2) return true;

        if (equivalentTerms.count(token1) &&
            find(equivalentTerms[token1].begin(), equivalentTerms[token1].end(), token2) != equivalentTerms[token1].end()) {
            return true;
        }

        if (token1.length() <= 3 || token2.length() <= 3) {
            return false;
        }

        int maxLength = max(token1.length(), token2.length());
        int threshold = maxLength <= 5 ? 1 : 2;

        return calculateLevenshteinDistance(token1, token2) <= threshold;
    }

    // Definition of method to calculate token overlap; takes two name strings as parameters; returns overlap score as double
    double LacerteCrossReference::tokenOverlap(const string& name1, const string& name2) {
        // Tokenize and sort both names
        auto tokens1 = tokenizeAndSort(preprocessName(name1));
        auto tokens2 = tokenizeAndSort(preprocessName(name2));

        // Convert to sets for unique tokens
        set<string> set1(tokens1.begin(), tokens1.end());
        set<string> set2(tokens2.begin(), tokens2.end());

        // Count matching tokens, including equivalent terms
        int matchCount = 0;
        for (const auto& token1 : set1) {
            for (const auto& token2 : set2) {
                if (areTokensEquivalent(token1, token2)) { 
                    matchCount++;
                    break;  // Move to next token1 once we find a match
                }
            }
        }

        // Calculate Jaccard similarity with equivalent terms considered
        int unionSize = set1.size() + set2.size() - matchCount;
        return unionSize > 0 ? static_cast<double>(matchCount) / unionSize : 0.0;
    }

    // Definition of method to load training data; takes filename string parameter; returns void
    void LacerteCrossReference::loadTrainingData(const string& filename) {
        auto csvData = readCSV(filename);
        training_data.clear();

        int positiveCount = 0;
        int negativeCount = 0;

        for (const auto& row : csvData) {
            if (row.size() >= 3) {
                TrainingPair pair;
                pair.system1_name = row[0];
                pair.system2_name = row[1];

                try {
                    // Handle both 1/-1 and 1/0 format in training data
                    int label = std::stoi(row[2]);

                    // Convert any negative number to false, any positive to true
                    pair.is_match = (label > 0);  // Store as boolean in memory

                    if (pair.is_match) {
                        positiveCount++;
                    } else {
                        negativeCount++;
                    }

                } catch (const std::exception& e) {
                    // Skip invalid rows 
                    continue;
                }

                training_data.push_back(pair);
            }
        }

        // If no data was loaded, throw an exception
        if (training_data.empty()) {
            throw runtime_error("No valid training data loaded from " + filename);
        }
    }

    // Definition of method to convert name to features; takes name string parameter; returns feature vector
    LacerteCrossReference::sample_type LacerteCrossReference::nameToFeatures(const string& name) {
        sample_type features;
        features.set_size(5);

        string processed = preprocessName(name);
        auto tokens = tokenizeAndSort(processed);

        // Feature 1: Word count (normalized)
        features(0) = std::min(static_cast<double>(tokens.size()) / 10.0, 1.0);

        // Feature 2: Contains legal entity
        features(1) = (processed.find("llc") != string::npos ||
                       processed.find("inc") != string::npos ||
                       processed.find("corp") != string::npos) ? 1.0 : 0.0;

        // Feature 3: Name length (normalized)
        features(2) = std::min(static_cast<double>(processed.length()) / 50.0, 1.0);

        // Feature 4: Initialize to 0 (will be set during comparison)
        features(3) = 0.0;

        // Feature 5: Special character ratio (normalized)
        int specialChars = count_if(name.begin(), name.end(),
                                    [](char c) { return !isalnum(c); });
        features(4) = static_cast<double>(specialChars) /
                      (processed.length() > 0 ? processed.length() : 1);

        return features;
    }

    // Definition of method to train the model; takes no parameters; returns void
    void LacerteCrossReference::trainModel() {
        vector<sample_type> samples;
        vector<double> labels;

        for (const auto& pair : training_data) {
            auto features1 = nameToFeatures(pair.system1_name);
            auto features2 = nameToFeatures(pair.system2_name);

            // Set token overlap for this specific pair
            double overlap = tokenOverlap(pair.system1_name, pair.system2_name);
            features1(3) = overlap;
            features2(3) = overlap;

            auto combined = concatenateFeatures(features1, features2);
            samples.push_back(combined);
            labels.push_back(pair.is_match ? 1.0 : -1.0);
        }

        dlib::svm_c_linear_trainer<kernel_type> trainer;
        trainer.set_c(10.0);
        trainer.set_epsilon(0.001);

        matcher_model = trainer.train(samples, labels);
    }

    // Definition of method to get match confidence; takes two name strings as parameters; returns confidence score as double
    double LacerteCrossReference::getMatchConfidence(const string& name1, const string& name2) {
        // Input validation
        if (name1.empty() || name2.empty()) {
            return 0.0;
        }

        try {
            // Preprocess both names
            string processed1 = preprocessName(name1);
            string processed2 = preprocessName(name2);

            // Early exit for exact matches after preprocessing
            if (processed1 == processed2) {
                return 1.0;
            }

            // Early exit for empty processed names
            if (processed1.empty() || processed2.empty()) {
                return 0.0;
            }

            // Get features for processed names
            auto features1 = nameToFeatures(processed1);
            auto features2 = nameToFeatures(processed2);

            // Set token overlap for this specific comparison
            double overlap = tokenOverlap(processed1, processed2);
            features1(3) = overlap;
            features2(3) = overlap;

            auto combined = concatenateFeatures(features1, features2);
            double raw_score = matcher_model(combined);

            // Convert to probability using sigmoid
            return 1.0 / (1.0 + std::exp(-raw_score));
        } catch (const exception& e) {
            // If any error occurs during processing, return 0 confidence
            return 0.0;
        }
    }

    // Definition of method to concatenate features; takes two feature vectors as parameters; returns combined feature vector
    LacerteCrossReference::sample_type LacerteCrossReference::concatenateFeatures(
            const sample_type& f1, const sample_type& f2) {
        sample_type combined;
        combined.set_size(f1.size() + f2.size());

        for (long i = 0; i < f1.size(); ++i) {
            combined(i) = f1(i);
        }
        for (long i = 0; i < f2.size(); ++i) {
            combined(i + f1.size()) = f2(i);
        }

        return combined;
    }

    // Definition of method to update model with feedback; takes names, match status, and confidence as parameters; returns void
    void LacerteCrossReference::updateModelWithFeedback(
            const string& name1, const string& name2,
            bool isCorrectMatch, double predictedConfidence,
            bool forceRetrain) {
        try {
            forceRetrain = false;
            cout << "\n=== Starting updateModelWithFeedback ===" << endl;
            cout << "Input Parameters:" << endl;
            cout << "  name1: " << name1 << endl;
            cout << "  name2: " << name2 << endl;
            cout << "  isCorrectMatch: " << isCorrectMatch << endl;
            cout << "  predictedConfidence: " << predictedConfidence << endl;
            cout << "  forceRetrain: " << (forceRetrain ? "yes" : "no") << endl;

            // Skip database storage if it's a force retrain call
            if (!forceRetrain) {
                cout << "\nAttempting to store feedback in database..." << endl;
                bool stored = database.storeFeedback(name1, name2, isCorrectMatch, predictedConfidence);
                if (!stored) {
                    cout << "Database storage failed!" << endl;
                    throw runtime_error("Failed to store feedback in database");
                }
                cout << "Feedback stored successfully in database" << endl;

                // Initialize metrics timestamp if this is the first prediction
                cout << "\nUpdating metrics..." << endl;
                cout << "Current total predictions: " << metrics.totalPredictions << endl;

                if (metrics.totalPredictions == 0) {
                    metrics.lastUpdate = chrono::system_clock::now();
                    cout << "Initialized metrics timestamp (first prediction)" << endl;
                }

                // Always increment total predictions for learning progress
                metrics.totalPredictions++;

                // Only update accuracy and matches for model predictions (predictedConfidence > 0)
                if (predictedConfidence > 0) {
                    double predictedLabel = predictedConfidence > 0.7 ? 1.0 : -1.0;
                    double actualLabel = isCorrectMatch ? 1.0 : -1.0;
                    bool wasCorrectPrediction = predictedLabel == actualLabel;

                    // Count high confidence predictions for accuracy calculation
                    if (predictedConfidence > 0.7) {  // Count all high confidence predictions
                        metrics.total_high_confidence++;  // Track total high confidence predictions
                    }

                    // Update match statistics - only count as match if it was correct
                    if (predictedConfidence > 0.7 && isCorrectMatch) {
                        metrics.matches_found++;
                        metrics.correct_matches++;
                    }

                    // Calculate accuracy after updating all counters
                    metrics.accuracy = metrics.total_high_confidence > 0 ? 
                        (double)metrics.correct_matches / metrics.total_high_confidence : 0.0;
                }

                // Track mismatches for retraining (include all feedback)
                if (!isCorrectMatch) {
                    metrics.recentMismatches.push_back({name1, name2});
                    if (metrics.recentMismatches.size() > learningParams.maxRecentMismatches) {
                        metrics.recentMismatches.erase(metrics.recentMismatches.begin());
                    }
                }
            }

            // Check if retraining is needed
            cout << "\nChecking if retraining is needed..." << endl;
            auto now = chrono::system_clock::now();
            bool needsRetrain = forceRetrain ||  // Added forceRetrain condition
                                metrics.recentMismatches.size() >= learningParams.minMismatchesForRetrain ||
                                chrono::duration_cast<chrono::hours>(now - metrics.lastUpdate).count() >= learningParams.hoursBeforeRetrain ||
                                metrics.accuracy < learningParams.retrainAccuracyThreshold;

            cout << "Retrain check results:" << endl;
            if (forceRetrain) cout << "  Force retrain requested" << endl;
            cout << "  Recent mismatches: " << metrics.recentMismatches.size()
                 << " (threshold: " << learningParams.minMismatchesForRetrain << ")" << endl;
            cout << "  Hours since update: "
                 << chrono::duration_cast<chrono::hours>(now - metrics.lastUpdate).count()
                 << " (threshold: " << learningParams.hoursBeforeRetrain << ")" << endl;
            cout << "  Current accuracy: " << metrics.accuracy
                 << " (threshold: " << learningParams.retrainAccuracyThreshold << ")" << endl;
            cout << "Needs retrain: " << (needsRetrain ? "yes" : "no") << endl;

            if (needsRetrain) {
                cout << "\nStarting model retraining..." << endl;
                cout << "Exporting feedback to training file..." << endl;
                database.exportFeedbackToTrainingFile("training_data.csv");

                cout << "Training model..." << endl;
                trainModel();

                metrics.lastUpdate = now;
                metrics.recentMismatches.clear();
                cout << "Model retraining completed" << endl;
            }

            cout << "=== updateModelWithFeedback completed successfully ===" << endl;

        } catch (const exception& e) {
            cout << "\nERROR in updateModelWithFeedback: " << e.what() << endl;
            throw runtime_error(string("Error in updateModelWithFeedback: ") + e.what());
        }
    }

    // Definition of method to get model metrics; takes no parameters; returns ModelMetrics object
    LacerteCrossReference::ModelMetrics LacerteCrossReference::getModelMetrics() const {
        return metrics;
    }

    // Definition of method to precompute database features; takes vector of projects parameter; returns vector of precomputed features
    vector<LacerteCrossReference::PrecomputedFeatures> LacerteCrossReference::precomputeDatabaseFeatures(
            const vector<Project>& projects) {
        vector<PrecomputedFeatures> precomputed;

        for (const auto& project : projects) {
            PrecomputedFeatures features;
            features.clientName = project.getClient();

            // Preprocess the name and compute features
            string processedName = preprocessName(features.clientName);
            features.features = nameToFeatures(processedName);

            // Store token information for faster matching
            auto tokens = tokenizeAndSort(processedName);

            // Add to precomputed vector
            precomputed.push_back(features);
        }

        return precomputed;
    }

} // namespace TaxReturnSystem