#pragma once
#include <string>
#include <vector>
#include <map>
#include "config.h"

using namespace std;

namespace TaxReturnSystem {

    // Class for handling Lacerte cross-referencing operations
    class LacerteCrossReference {
    private:
        map<string, vector<string>> equivalentTerms; // Map of equivalent terms for matching

        // CSV and data processing methods
        vector<vector<string>> readCSV(const string& filename); // Read data from CSV file
        string preprocessName(const string& name); // Preprocess name for comparison
        vector<string> tokenizeAndSort(const string& name); // Split and sort name tokens
        double tokenOverlap(const string& name1, const string& name2); // Calculate token overlap ratio
        void generateReport(const vector<vector<string>>& reportData); // Generate comparison report

        // Term matching methods
        void initializeEquivalentTerms(); // Initialize equivalent terms dictionary
        bool areTokensEquivalent(const string& token1, const string& token2); // Check if tokens are equivalent
        int calculateLevenshteinDistance(const string& s1, const string& s2); // Calculate string distance

    public:
        LacerteCrossReference(); // Constructor
        void crossReferenceData(const vector<vector<string>>& lacerteData, const Data& csvData); // Cross-reference Lacerte and CSV data
    };

} // namespace TaxReturnSystem