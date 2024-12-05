/**
 * @file Lacerte_cross_ref.cpp
 * @brief Implementation of Lacerte cross-reference functionality for the Tax Return System
 *
 * This file contains implementations for:
 * - Initializing equivalent terms for name comparison
 * - Reading and processing CSV data
 * - Preprocessing and tokenizing names
 * - Calculating Levenshtein distance for string similarity
 * - Cross-referencing Lacerte and CSV data
 * - Generating Excel reports from cross-reference results
 */

#include "Lacerte_cross_ref.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <cctype>
#include <regex>
#include "xlsxwriter.h" // For Excel file generation
#include "config.h"

using namespace std;

namespace TaxReturnSystem {

    // Definition of a constructor to initialize equivalent terms; takes no parameters; returns nothing
    LacerteCrossReference::LacerteCrossReference() {
        initializeEquivalentTerms();
    }

    // Definition of a method to initialize equivalent terms mapping; takes no parameters; returns void
    void LacerteCrossReference::initializeEquivalentTerms() {
        // Initialize map with common abbreviations and their full forms
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
        };
    }

    // Definition of a method to read data from a CSV file; takes a string as parameter; returns a vector of vector of strings
    vector<vector<string>> LacerteCrossReference::readCSV(const string& filename) {
        vector<vector<string>> data;
        ifstream file(filename);
        string line;

        // Read each line from the file
        while (getline(file, line)) {
            stringstream ss(line);
            string item;
            vector<string> row;

            // Split line into items by comma
            while (getline(ss, item, ',')) {
                row.push_back(item);
            }
            data.push_back(row);
        }
        return data;
    }

    // Definition of a method to preprocess a name for comparison; takes a string as parameter; returns a string
    string LacerteCrossReference::preprocessName(const string& name) {
        // Remove punctuation marks
        string processed = regex_replace(name, regex("[.,-]"), "");

        // Normalize whitespace to single spaces
        processed = regex_replace(processed, regex("\\s+"), " ");

        // Convert all characters to lowercase
        transform(processed.begin(), processed.end(), processed.begin(), ::tolower);

        return processed;
    }

    // Definition of a method to tokenize and sort a name; takes a string as parameter; returns vector of strings
    vector<string> LacerteCrossReference::tokenizeAndSort(const string& name) {
        vector<string> tokens;
        stringstream ss(name);
        string token;

        // Split name into individual tokens
        while (ss >> token) {
            tokens.push_back(token);
        }

        // Sort tokens alphabetically
        sort(tokens.begin(), tokens.end());

        return tokens;
    }

    // Definition of a method to calculate Levenshtein distance between two strings; takes two strings as parameters; returns int
    int LacerteCrossReference::calculateLevenshteinDistance(const string& s1, const string& s2) {
        // Initialize DP table with dimensions based on input strings
        vector<vector<int>> dp(s1.length() + 1, vector<int>(s2.length() + 1));

        // Base case initialization
        for (int i = 0; i <= s1.length(); i++)
            dp[i][0] = i;
        for (int j = 0; j <= s2.length(); j++)
            dp[0][j] = j;

        // Fill DP table using recurrence relation
        for (int i = 1; i <= s1.length(); i++) {
            for (int j = 1; j <= s2.length(); j++) {
                if (s1[i-1] == s2[j-1])
                    dp[i][j] = dp[i-1][j-1]; // No operation needed
                else
                    dp[i][j] = 1 + min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]}); // Min of insert, delete, replace
            }
        }

        // Return the computed Levenshtein distance
        return dp[s1.length()][s2.length()];
    }

    // Definition of a method to check if two tokens are equivalent; takes two strings as parameters; returns bool
    bool LacerteCrossReference::areTokensEquivalent(const string& token1, const string& token2) {
        // Check for exact string match
        if (token1 == token2) return true;

        // Check if tokens are listed as equivalent in dictionary
        if (equivalentTerms.count(token1) &&
            find(equivalentTerms[token1].begin(), equivalentTerms[token1].end(), token2) != equivalentTerms[token1].end()) {
            return true;
        }

        // Skip Levenshtein check for short tokens (abbreviations)
        if (token1.length() <= 3 || token2.length() <= 3) {
            return false;
        }

        // For longer tokens, use Levenshtein distance with adaptive threshold
        int maxLength = max(token1.length(), token2.length());
        int threshold = maxLength <= 5 ? 1 : 2;  // Higher threshold for longer words

        return calculateLevenshteinDistance(token1, token2) <= threshold;
    }

    // Definition of a method to calculate token overlap between two names; takes two strings as parameters; returns double
    double LacerteCrossReference::tokenOverlap(const string& name1, const string& name2) {
        // Tokenize and preprocess both names
        auto tokens1 = tokenizeAndSort(preprocessName(name1));
        auto tokens2 = tokenizeAndSort(preprocessName(name2));

        // Convert token vectors to sets for unique tokens
        set<string> set1(tokens1.begin(), tokens1.end());
        set<string> set2(tokens2.begin(), tokens2.end());

        int matchCount = 0;
        // Count equivalent tokens between the two sets
        for (const auto& token1 : set1) {
            for (const auto& token2 : set2) {
                if (areTokensEquivalent(token1, token2)) {
                    matchCount++;
                    break;
                }
            }
        }

        // Calculate and return the token overlap ratio
        return static_cast<double>(matchCount) / (set1.size() + set2.size() - matchCount);
    }

    // Definition of a method to cross reference Lacerte and CSV data; takes a vector of vectors of strings and a Data object as parameters; returns void
    void LacerteCrossReference::crossReferenceData(const vector<vector<string>>& lacerteData, const Data& csvData) {
        // Create map of Lacerte data with client name as key
        map<string, vector<string>> lacerteMap;
        for (const auto& row : lacerteData) {
            string clientName = row[1]; // Client Name
            lacerteMap[clientName] = row;
        }

        vector<vector<string>> reportData;

        // Compare each CSV entry with Lacerte data
        for (const auto& entry : csvData.getFilteredMap()) {
            const Row& row = entry.second;
            string csvClientName = row.getCellValue(CLIENT_COLUMN);

            // Calculate name similarity with first Lacerte entry
            double similarity = tokenOverlap(csvClientName, lacerteMap.begin()->first);

            // Process entries with high similarity
            if (similarity > 0.8) {
                auto it = lacerteMap.find(csvClientName);
                if (it != lacerteMap.end()) {
                    const vector<string>& lacerteRow = it->second;

                    // Check extension status
                    string federalExtStatus = lacerteRow[5];
                    string stateExtStatus = lacerteRow[6];
                    string overallExtStatus = (federalExtStatus == "Accepted" || stateExtStatus == "Accepted") ? "Extended" : "Not Extended";

                    // Compare extension status with CSV tag
                    string tag = row.getCellValue(BILLING_PARTNER_COLUMN);
                    bool extStatusMatch = (overallExtStatus == "Extended" && tag.find("Extended") != string::npos);

                    // Check filing status
                    string efileStatus = lacerteRow[7];
                    string stateEfileStatus = lacerteRow[8];
                    string overallFilingStatus = (efileStatus == "Accepted" || stateEfileStatus == "Accepted") ? "Filed" : "Not Filed";

                    // Compare filing status with CSV next task
                    string nextTask = row.getCellValue(NEXT_TASK_COLUMN);
                    bool filingStatusMatch = (overallFilingStatus == "Filed" && nextTask == "Tax Return Filed");

                    // Add comparison results to report data
                    reportData.push_back({csvClientName, overallExtStatus, overallFilingStatus,
                                          extStatusMatch ? "Match" : "No Match",
                                          filingStatusMatch ? "Match" : "No Match"});
                }
            }
        }

        // Generate final report
        generateReport(reportData);
    }

    // Definition of a method to generate Excel report from cross reference data; takes a vector of vectors of strings as parameter; returns void
    void LacerteCrossReference::generateReport(const vector<vector<string>>& reportData) {
        // Create new Excel workbook and worksheet
        lxw_workbook  *workbook = workbook_new("CrossReferenceReport.xlsx");
        lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);

        // Write column headers
        worksheet_write_string(worksheet, 0, 0, "Client Name", NULL);
        worksheet_write_string(worksheet, 0, 1, "Overall Ext Status", NULL);
        worksheet_write_string(worksheet, 0, 2, "Overall Filing Status", NULL);
        worksheet_write_string(worksheet, 0, 3, "Extension Status Match", NULL);
        worksheet_write_string(worksheet, 0, 4, "Filing Status Match", NULL);

        // Write report data row by row
        for (size_t i = 0; i < reportData.size(); ++i) {
            for (size_t j = 0; j < reportData[i].size(); ++j) {
                worksheet_write_string(worksheet, i + 1, j, reportData[i][j].c_str(), NULL);
            }
        }

        // Save and close workbook
        workbook_close(workbook);
    }

} // namespace TaxReturnSystem