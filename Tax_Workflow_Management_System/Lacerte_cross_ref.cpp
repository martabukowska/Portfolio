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
#include <thread>
#include <atomic>

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
        static const unordered_map<string, string> businessAbbrs = {
                {"llc", ""}, {"ltd", ""}, {"inc", ""}, {"corp", ""},
                {"corporation", ""}, {"incorporated", ""}, {"limited", ""},
                {"company", ""}, {"co", ""}, {"lp", ""}, {"llp", ""},
                {"l.l.c.", ""}, {"l.t.d.", ""}, {"l.p.", ""}, {"l.l.p.", ""}
        };

        string processed;
        processed.reserve(name.length());

        transform(name.begin(), name.end(), back_inserter(processed),
                  [](unsigned char c) { return tolower(c); });

        static const regex cleanup_pattern("[.,;:'\"\\-_\\s]+");
        processed = regex_replace(processed, cleanup_pattern, " ");

        for (const auto& [abbr, _] : businessAbbrs) {
            size_t pos;
            while ((pos = processed.find(abbr)) != string::npos) {
                processed.erase(pos, abbr.length());
            }
        }

        if (!processed.empty()) {
            size_t start = processed.find_first_not_of(" ");
            size_t end = processed.find_last_not_of(" ");
            if (start != string::npos && end != string::npos) {
                processed = processed.substr(start, end - start + 1);
            }
        }

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
    double LacerteCrossReference::tokenOverlap(const PrecomputedFeatures& features1, const PrecomputedFeatures& features2) {
        int matchCount = 0;

        const auto& smaller = features1.tokenSet.size() < features2.tokenSet.size() ?
                              features1.tokenSet : features2.tokenSet;
        const auto& larger = features1.tokenSet.size() < features2.tokenSet.size() ?
                             features2.tokenSet : features1.tokenSet;

        for (const auto& token : smaller) {
            if (larger.count(token) ||
                any_of(larger.begin(), larger.end(),
                       [&](const string& t) { return areTokensEquivalent(token, t); })) {
                matchCount++;
            }
        }

        int unionSize = features1.tokenSet.size() + features2.tokenSet.size() - matchCount;
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
                    int label = std::stoi(row[2]);

                    pair.is_match = (label > 0);

                    if (pair.is_match) {
                        positiveCount++;
                    } else {
                        negativeCount++;
                    }

                } catch (const std::exception& e) {
                    continue;
                }

                training_data.push_back(pair);
            }
        }

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
            // Create PrecomputedFeatures for both names
            PrecomputedFeatures features1;
            features1.clientName = pair.system1_name;
            features1.processedName = preprocessName(pair.system1_name);
            features1.tokens = tokenizeAndSort(features1.processedName);
            features1.tokenSet = unordered_set<string>(features1.tokens.begin(), features1.tokens.end());
            features1.features = nameToFeatures(features1.processedName);

            PrecomputedFeatures features2;
            features2.clientName = pair.system2_name;
            features2.processedName = preprocessName(pair.system2_name);
            features2.tokens = tokenizeAndSort(features2.processedName);
            features2.tokenSet = unordered_set<string>(features2.tokens.begin(), features2.tokens.end());
            features2.features = nameToFeatures(features2.processedName);

            // Set token overlap for this specific pair
            double overlap = tokenOverlap(features1, features2);
            features1.features(3) = overlap;
            features2.features(3) = overlap;

            auto combined = concatenateFeatures(features1.features, features2.features);
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
        // Create PrecomputedFeatures from strings
        PrecomputedFeatures features1;
        features1.clientName = name1;
        features1.processedName = preprocessName(name1);
        features1.tokens = tokenizeAndSort(features1.processedName);
        features1.tokenSet = unordered_set<string>(features1.tokens.begin(), features1.tokens.end());
        features1.features = nameToFeatures(features1.processedName);

        PrecomputedFeatures features2;
        features2.clientName = name2;
        features2.processedName = preprocessName(name2);
        features2.tokens = tokenizeAndSort(features2.processedName);
        features2.tokenSet = unordered_set<string>(features2.tokens.begin(), features2.tokens.end());
        features2.features = nameToFeatures(features2.processedName);

        return getMatchConfidence(features1, features2);
    }

    double LacerteCrossReference::getMatchConfidence(const PrecomputedFeatures& features1,const PrecomputedFeatures& features2) {
        // Early exit for exact matches
        if (features1.processedName == features2.processedName) {
            return 1.0;
        }

        // Early exit for empty processed names
        if (features1.processedName.empty() || features2.processedName.empty()) {
            return 0.0;
        }

        // Set token overlap for this specific comparison
        double overlap = tokenOverlap(features1, features2);
        auto f1 = features1.features;
        auto f2 = features2.features;
        f1(3) = overlap;
        f2(3) = overlap;

        auto combined = concatenateFeatures(f1, f2);
        double raw_score = matcher_model(combined);

        // Convert to probability using sigmoid
        return 1.0 / (1.0 + std::exp(-raw_score));
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
                        metrics.totalHighConfidence++;  // Track total high confidence predictions
                    }

                    // Update match statistics - only count as match if it was correct
                    if (predictedConfidence > 0.7 && isCorrectMatch) {
                        metrics.matchesFound++;
                        metrics.correctMatches++;
                    }

                    // Calculate accuracy after updating all counters
                    metrics.accuracy = metrics.totalHighConfidence > 0 ?
                                       (double)metrics.correctMatches / metrics.totalHighConfidence : 0.0;
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

    vector<MatchResult> LacerteCrossReference::findMatches(
            const vector<string>& lacerteNames,
            const vector<PrecomputedFeatures>& precomputed) {

        vector<MatchResult> results(lacerteNames.size());
        mutex cout_mutex;
        const double EARLY_EXIT_THRESHOLD = 0.95;

        // Create length-based buckets
        unordered_map<size_t, vector<size_t>> lengthBuckets;
        for (size_t idx = 0; idx < precomputed.size(); idx++) {
            size_t lengthBucket = precomputed[idx].clientName.length() / 5; // Group in ranges of 5
            lengthBuckets[lengthBucket].push_back(idx);
        }

        const size_t num_threads = thread::hardware_concurrency();
        vector<thread> threads;
        const size_t lacerte_chunk_size = max(size_t(1), lacerteNames.size() / num_threads);

        vector<atomic<size_t>> thread_progress(num_threads);
        vector<atomic<size_t>> early_exits(num_threads);
        vector<atomic<size_t>> comparisons_saved(num_threads);

        for (size_t t = 0; t < num_threads; ++t) {
            const size_t lacerte_start = t * lacerte_chunk_size;
            const size_t lacerte_end = (t == num_threads - 1) ?
                                       lacerteNames.size() : min((t + 1) * lacerte_chunk_size, lacerteNames.size());

            if (lacerte_start >= lacerte_end) continue;

            threads.emplace_back([&, t, lacerte_start, lacerte_end]() {
                size_t local_early_exits = 0;
                size_t local_comparisons = 0;
                size_t total_possible_comparisons = 0;

                for (size_t i = lacerte_start; i < lacerte_end; ++i) {
                    const string& lacerteName = lacerteNames[i];
                    double bestConfidence = 0.0;
                    string bestMatch;

                    // Get candidates from length buckets
                    set<size_t> candidateIndices;
                    size_t currentBucket = lacerteName.length() / 5;

                    // Check current bucket and adjacent buckets
                    for (int bucketOffset = -1; bucketOffset <= 1; bucketOffset++) {
                        size_t bucket = currentBucket + bucketOffset;
                        if (lengthBuckets.find(bucket) != lengthBuckets.end()) {
                            const auto& indices = lengthBuckets[bucket];
                            candidateIndices.insert(indices.begin(), indices.end());
                        }
                    }

                    total_possible_comparisons += precomputed.size();
                    local_comparisons += candidateIndices.size();

                    // Compare with candidates
                    for (size_t idx : candidateIndices) {
                        if (idx >= precomputed.size()) continue; // Safety check

                        double confidence = getMatchConfidence(lacerteName, precomputed[idx].clientName);
                        if (confidence > bestConfidence) {
                            bestConfidence = confidence;
                            bestMatch = precomputed[idx].clientName;

                            if (confidence >= EARLY_EXIT_THRESHOLD) {
                                local_early_exits++;
                                break;
                            }
                        }
                    }

                    results[i] = {lacerteName, bestMatch, bestConfidence};
                    thread_progress[t]++;

                    // Progress reporting
                    size_t report_interval = min(size_t(10), max(size_t(1), (lacerte_end - lacerte_start) / 20));
                    if (thread_progress[t] % report_interval == 0) {
                        lock_guard<mutex> lock(cout_mutex);
                        double progress = (100.0 * thread_progress[t]) / (lacerte_end - lacerte_start);
                        double comparison_reduction = 100.0 * (1.0 - (double)local_comparisons / total_possible_comparisons);
                        cout << "Thread " << t << ": "
                             << thread_progress[t] << "/" << (lacerte_end - lacerte_start)
                             << " (" << fixed << setprecision(1) << progress << "%) "
                             << "Comparisons reduced by " << comparison_reduction << "%" << endl;
                    }
                }

                early_exits[t] = local_early_exits;
                comparisons_saved[t] = total_possible_comparisons - local_comparisons;

                {
                    lock_guard<mutex> lock(cout_mutex);
                    double comparison_reduction = 100.0 * (1.0 - (double)local_comparisons / total_possible_comparisons);
                    cout << "Thread " << t << " completed. "
                         << thread_progress[t] << " names processed, "
                         << local_early_exits << " early exits, "
                         << "comparisons reduced by " << comparison_reduction << "%" << endl;
                }
            });
        }

        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }

        // Final summary
        cout << "\nAll threads completed. Summary:" << endl;
        size_t total_processed = 0;
        size_t total_early_exits = 0;
        size_t total_comparisons_saved = 0;
        size_t total_possible = 0;

        for (size_t t = 0; t < num_threads; ++t) {
            total_processed += thread_progress[t];
            total_early_exits += early_exits[t];
            total_comparisons_saved += comparisons_saved[t];
            total_possible += thread_progress[t] * precomputed.size();
        }

        double overall_reduction = 100.0 * (double)total_comparisons_saved / total_possible;
        cout << "\nTotal processed: " << total_processed << "/" << lacerteNames.size() << endl;
        cout << "Total early exits: " << total_early_exits
             << " (" << (100.0 * total_early_exits / total_processed) << "%)" << endl;
        cout << "Overall comparison reduction: " << overall_reduction << "%" << endl;

        return results;
    }

    // Definition of method to test database access; takes no parameters; returns boolean indicating success
    bool LacerteCrossReference::testDatabaseAccess() {
        try {
            string testName1 = "Test Company ABC";
            string testName2 = "Test Company XYZ";
            bool isMatch = true;
            double confidence = 0.85;

            return database.storeFeedback(testName1, testName2, isMatch, confidence);
        } catch (const exception& e) {
            cout << "Database access error: " << e.what() << endl;
            return false;
        }
    }

    // Definition of method to benchmark precompute performance; takes vector of projects parameter; returns void
    void LacerteCrossReference::benchmarkPrecompute(const vector<Project>& projects) {
        cout << "\n=== Running Precompute Benchmark ===" << endl;

        auto startTime = chrono::high_resolution_clock::now();

        cout << "Starting precomputation for " << projects.size() << " projects..." << endl;
        auto precomputed = precomputeDatabaseFeatures(projects);

        auto endTime = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

        double timeSeconds = duration.count() / 1000.0;
        double itemsPerSecond = projects.size() / timeSeconds;

        cout << "\nBenchmark Results:" << endl;
        cout << "--------------------------------" << endl;
        cout << "Total time: " << timeSeconds << " seconds" << endl;
        cout << "Items processed: " << projects.size() << endl;
        cout << "Processing speed: " << itemsPerSecond << " items/second" << endl;
        cout << "Threads used: " << thread::hardware_concurrency() - 1 << endl;
        cout << "--------------------------------" << endl;
    }

} // namespace TaxReturnSystem