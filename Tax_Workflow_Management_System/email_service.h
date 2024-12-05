#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

namespace TaxReturnSystem {

    class EmailService {
    private:
        void logEmail(const string& to, const string& subject, const string& body); // Log email sending attempts

    public:
        // Email operations
        bool sendEmail(const string& to, const string& subject, const string& body); // Send email and return success status
    };

} // namespace TaxReturnSystem