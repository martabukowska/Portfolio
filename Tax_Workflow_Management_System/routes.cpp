/**
 * @file routes.cpp
 * @brief Implementation of HTTP routes for the Tax Return System web API
 *
 * This file implements all API endpoints including:
 * - Authentication (login, logout, registration)
 * - Password management (reset, change)
 * - Project management (search, filter, statistics)
 * - CSV data import
 * - User settings management
 *
 * The routes are implemented using the Crow framework and handle:
 * - CORS headers
 * - Authentication validation
 * - Request validation
 * - Error handling
 * - JSON responses
 */

#include "routes.h"
#include "report_generator.h"
#include "statistics.h"
#include <functional>
#include "crow/mustache.h"

using namespace std;

using namespace TaxReturnSystem;

// Helper function to add CORS headers
void addCorsHeaders(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

void setupRoutes(crow::SimpleApp& app, Auth& auth, ReminderSystem& reminderSystem, ProjectManager& projectManager) {

    // Definition of a route to handle CORS preflight OPTIONS requests; takes path parameter; returns 204 status
    CROW_ROUTE(app, "/<path>").methods("OPTIONS"_method)
            ([](const crow::request& req, crow::response& res, string path) {
                // Add CORS headers to response
                addCorsHeaders(res);

                // Set "No Content" status code and end response
                res.code = 204;
                res.end();
            });

    // Definition of a route to serve the main index page; takes no parameters; returns HTML response
    CROW_ROUTE(app, "/")([]() {
        crow::response res;

        // Add CORS and cache control headers
        addCorsHeaders(res);
        res.add_header("Cache-Control", "no-cache, no-store, must-revalidate");
        res.add_header("Pragma", "no-cache");
        res.add_header("Expires", "0");

        // Read and serve index.html file
        ifstream file("templates/index.html");
        if (file) {
            // Load file contents and set successful response
            string contents((std::istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            res.body = contents;
            res.code = 200;
            res.set_header("Content-Type", "text/html");
        } else {
            // Set 404 response if file not found
            res.body = "File not found";
            res.code = 404;
        }
        return res;
    });

    // Definition of a route to serve the dashboard page; takes a request parameter; returns HTML response
    CROW_ROUTE(app, "/dashboard.html")([](const crow::request& req) {
        // Initialize response with success status and headers
        crow::response res;
        res.code = 200;
        res.set_header("Content-Type", "text/html");
        addCorsHeaders(res);

        // Read and serve dashboard.html file
        ifstream file("templates/dashboard.html");
        if (file) {
            // Load file contents for successful response
            string contents((std::istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            res.body = contents;
        } else {
            // Set 404 response if file not found
            res.body = "Dashboard HTML file not found";
            res.code = 404;
        }

        return res;
    });

    // Definition of a route to get filter options for projects; takes a request parameter; returns JSON response
    CROW_ROUTE(app, "/filter-options").methods("GET"_method)
            ([&auth, &projectManager](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Validate authentication token
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    // Get all projects and log count
                    vector<Project> projects = projectManager.getAllProjects();
                    cout << "Total projects for filter options: " << projects.size() << endl;

                    // Initialize sets for unique filter values
                    set<string> groups;
                    set<string> projectTypes;
                    set<string> managers;
                    set<string> partners;

                    // Collect unique values from all projects
                    for (const auto& project : projects) {
                        if (!project.getGroup().empty()) groups.insert(project.getGroup());
                        if (!project.getProjectType().empty()) projectTypes.insert(project.getProjectType());
                        if (!project.getManager().empty()) managers.insert(project.getManager());
                        if (!project.getPartner().empty()) {
                            cout << "Adding partner: '" << project.getPartner() << "'" << endl;
                            partners.insert(project.getPartner());
                        }
                    }

                    // Debug output for partners
                    cout << "Found unique partners: " << partners.size() << endl;
                    for (const auto& partner : partners) {
                        cout << "Partner in set: '" << partner << "'" << endl;
                    }

                    // Build JSON response with filter options
                    crow::json::wvalue response_body;
                    response_body["groups"] = vector<string>(groups.begin(), groups.end());
                    response_body["projectTypes"] = vector<string>(projectTypes.begin(), projectTypes.end());
                    response_body["managers"] = vector<string>(managers.begin(), managers.end());
                    response_body["partners"] = vector<string>(partners.begin(), partners.end());

                    // Set successful response
                    res.body = response_body.dump();
                    res.code = 200;
                    res.add_header("Content-Type", "application/json");
                } catch (const exception& e) {
                    // Handle errors with 500 response
                    res.code = 500;
                    res.body = string("Error retrieving filter options: ") + e.what();
                }

                return res;
            });

    // Definition of a route to handle user login; takes a request parameter; returns JSON response with auth token
    CROW_ROUTE(app, "/login").methods("POST"_method)
            ([&auth](const crow::request& req) {
                // Initialize response with CORS headers
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Parse and validate JSON request body
                    auto x = crow::json::load(req.body);
                    if (!x) {
                        res.code = 400;
                        res.body = "Invalid JSON";
                        return res;
                    }

                    // Extract credentials from request
                    string username = x["username"].s();
                    string password = x["password"].s();

                    // Attempt login and get result
                    LoginResult result = auth.loginUser(username, password);

                    // Handle different login outcomes
                    switch(result) {
                        case LoginResult::InvalidUsername:
                            res.code = 400;
                            res.body = "Invalid username format";
                            break;
                        case LoginResult::InvalidPasswordLength:
                            res.code = 400;
                            res.body = "Invalid password length";
                            break;
                        case LoginResult::AccountLocked:
                            res.code = 403;
                            res.body = "Account is locked. Please try again later.";
                            break;
                        case LoginResult::InvalidCredentials:
                            res.code = 401;
                            res.body = "Invalid credentials";
                            break;
                        case LoginResult::Success:
                            // Create and return auth token for successful login
                            res.code = 200;
                            crow::json::wvalue response_body;
                            response_body["token"] = auth.createTokenForUser(username);
                            res.body = response_body.dump();
                            res.add_header("Content-Type", "application/json");
                            break;
                    }
                } catch (const exception& e) {
                    // Handle unexpected errors
                    res.code = 500;
                    res.body = string("Internal server error: ") + e.what();
                }

                return res;
            });

    // Definition of a route to handle user registration; takes a request parameter; returns JSON response
    CROW_ROUTE(app, "/register").methods("POST"_method)
            ([&auth](const crow::request& req) {
                // Initialize response with CORS headers
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Parse and validate JSON request body
                    auto x = crow::json::load(req.body);
                    if (!x) {
                        res.code = 400;
                        res.body = crow::json::wvalue({{"success", false}, {"message", "Invalid JSON"}}).dump();
                        return res;
                    }

                    // Extract registration details from request
                    string username = x["username"].s();
                    string email = x["email"].s();
                    string password = x["password"].s();

                    // Attempt user registration
                    if (auth.registerUser(username, password, email)) {
                        // Return success response
                        res.code = 201;
                        res.body = crow::json::wvalue({{"success", true}, {"message", "User registered successfully"}}).dump();
                    } else {
                        // Return failure response
                        res.code = 400;
                        res.body = crow::json::wvalue({{"success", false}, {"message", "Registration failed"}}).dump();
                    }
                } catch (const exception& e) {
                    // Handle unexpected errors
                    res.code = 500;
                    res.body = crow::json::wvalue({{"success", false}, {"message", string("Internal server error: ") + e.what()}}).dump();
                }

                // Set JSON content type and return response
                res.add_header("Content-Type", "application/json");
                return res;
            });

    // Definition of a route to handle password reset requests; takes a request parameter; returns JSON response
    CROW_ROUTE(app, "/reset-password-request").methods("POST"_method)
            ([&auth](const crow::request& req) {
                // Initialize response with CORS headers
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Parse and validate JSON request body
                    auto x = crow::json::load(req.body);
                    if (!x) {
                        res.code = 400;
                        res.body = crow::json::wvalue({{"success", false}, {"message", "Invalid JSON"}}).dump();
                        return res;
                    }

                    // Extract email from request
                    string email = x["email"].s();

                    // Attempt password reset request
                    bool result = auth.requestPasswordReset(email);

                    if (result) {
                        // Return success response
                        res.code = 200;
                        res.body = crow::json::wvalue({
                                                              {"success", true},
                                                              {"message", "Password reset request successful. Check your email for further instructions."}
                                                      }).dump();
                    } else {
                        // Return failure response
                        res.code = 400;
                        res.body = crow::json::wvalue({
                                                              {"success", false},
                                                              {"message", "Password reset request failed."}
                                                      }).dump();
                    }
                } catch (const exception& e) {
                    // Handle unexpected errors
                    res.code = 500;
                    res.body = crow::json::wvalue({
                                                          {"success", false},
                                                          {"message", string("Internal server error: ") + e.what()}
                                                  }).dump();
                }

                // Set JSON content type and return response
                res.add_header("Content-Type", "application/json");
                return res;
            });

    // Definition of a route to confirm password reset; takes a request parameter; returns success/failure response
    CROW_ROUTE(app, "/reset-password-confirm").methods("POST"_method)
            ([&auth](const crow::request& req) {
                // Initialize response with CORS headers
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Parse and validate JSON request body
                    auto x = crow::json::load(req.body);
                    if (!x || !x.has("email") || !x.has("token") || !x.has("newPassword")) {
                        res.code = 400;
                        res.body = "Invalid JSON or missing required fields";
                        return res;
                    }

                    // Extract reset confirmation details
                    string email = x["email"].s();
                    string token = x["token"].s();
                    string newPassword = x["newPassword"].s();

                    // Attempt password reset
                    bool result = auth.resetPassword(email, token, newPassword);

                    if (result) {
                        // Return success response
                        res.code = 200;
                        res.body = "Password reset successful.";
                    } else {
                        // Return failure response
                        res.code = 400;
                        res.body = "Password reset failed.";
                    }
                } catch (const exception& e) {
                    // Handle unexpected errors
                    res.code = 500;
                    res.body = string("Internal server error: ") + e.what();
                }

                return res;
            });

    // Definition of a route to update user email; takes a request parameter; returns success/failure response
    CROW_ROUTE(app, "/user/update-email").methods("POST"_method)
            ([&auth](const crow::request& req) {
                // Initialize response with CORS headers
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Extract and validate authentication token
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") token = token.substr(7);
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    // Parse and validate JSON request body
                    auto x = crow::json::load(req.body);
                    if (!x || !x.has("newEmail")) {
                        res.code = 400;
                        res.body = "Invalid JSON or missing parameters";
                        return res;
                    }

                    // Extract new email from request
                    string newEmail = x["newEmail"].s();

                    // Update user's email
                    User user = auth.getUserFromToken(token);
                    user.setEmail(newEmail);

                    // Verify update and send response
                    if (user.getEmail() == newEmail) {
                        res.code = 200;
                        res.body = "User email updated successfully";
                    } else {
                        res.code = 400;
                        res.body = "Failed to update user email";
                    }
                } catch (const exception& e) {
                    // Handle unexpected errors
                    res.code = 500;
                    res.body = string("Internal server error: ") + e.what();
                }

                return res;
            });

    // Definition of a route to handle CSV data import for admins; takes a request parameter; returns import status response
    CROW_ROUTE(app, "/admin/load-csv").methods("POST"_method)
            ([&auth, &projectManager](const crow::request& req) {
                // Initialize response with CORS headers
                crow::response res;
                addCorsHeaders(res);

                try {
                    cout << "=== Starting CSV Import Route Handler ===" << endl;

                    // Extract and validate authentication token
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    // Verify admin privileges
                    User user = auth.getUserFromToken(token);
                    if (user.getRole() != UserRole::Admin) {
                        res.code = 403;
                        res.body = "Unauthorized access";
                        return res;
                    }

                    // Process request based on content type
                    string contentType = req.get_header_value("Content-Type");
                    ReportType reportType = ReportType::RegularDeadline;
                    string filename;

                    if (contentType.find("application/json") != string::npos) {
                        // Parse JSON request and extract parameters
                        auto x = crow::json::load(req.body);
                        if (!x || !x.has("filename")) {
                            res.code = 400;
                            res.body = "Invalid JSON or missing filename";
                            return res;
                        }
                        filename = x["filename"].s();
                        if (x.has("reportType")) {
                            reportType = static_cast<ReportType>(x["reportType"].i());
                        }

                        // Log import attempt details
                        cout << "Attempting to import CSV file: " << filename << endl;
                        cout << "Report type: " << static_cast<int>(reportType) << endl;

                        // Perform CSV import
                        bool success = projectManager.importFromCSV(filename, reportType);
                        cout << "Import result: " << (success ? "success" : "failure") << endl;

                        if (!success) {
                            res.code = 500;
                            res.body = "Failed to import CSV data";
                            return res;
                        }

                        // Get and log import results
                        vector<Project> projects = projectManager.getAllProjects();
                        cout << "Total projects after import: " << projects.size() << endl;

                        // Set success response
                        res.code = 200;
                        res.body = "CSV data loaded successfully. Imported " + to_string(projects.size()) + " projects.";
                    } else {
                        // Handle unsupported content type
                        res.code = 400;
                        res.body = "Unsupported Content-Type";
                    }
                } catch (const exception& e) {
                    // Handle unexpected errors
                    res.code = 500;
                    res.body = string("Error loading CSV data: ") + e.what();
                    cout << "Exception during CSV import: " << e.what() << endl;
                }

                cout << "=== Ending CSV Import Route Handler ===" << endl;
                return res;
            });

    // Definition of a route to get all project data; takes a request parameter; returns JSON array of projects
    CROW_ROUTE(app, "/data").methods("GET"_method)
            ([&auth, &projectManager](const crow::request& req) {
                // Initialize response with CORS headers
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Extract and validate authentication token
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    // Retrieve all projects
                    vector<Project> projects = projectManager.getAllProjects();
                    crow::json::wvalue response_body;
                    int i = 0;

                    // Convert each project to JSON format
                    for (const auto& project : projects) {
                        crow::json::wvalue projectJson;
                        projectJson["id"] = project.getId();
                        projectJson["group"] = project.getGroup();
                        projectJson["client"] = project.getClient();
                        projectJson["projectType"] = project.getProjectType();
                        projectJson["billingPartner"] = project.getBillingPartner();
                        projectJson["partner"] = project.getPartner();
                        projectJson["manager"] = project.getManager();
                        projectJson["nextTask"] = project.getNextTask();
                        projectJson["memo"] = project.getMemo();
                        projectJson["regularDeadline"] = project.getRegularDeadline().getDateStr();
                        projectJson["internalDeadline"] = project.getInternalDeadline().getDateStr();
                        projectJson["extended"] = project.isExtended();
                        projectJson["reportType"] = static_cast<int>(project.getReportType());

                        // Add project to response array
                        response_body[i++] = move(projectJson);
                    }

                    // Set successful response with JSON data
                    res.body = response_body.dump();
                    res.code = 200;
                    res.add_header("Content-Type", "application/json");
                } catch (const exception& e) {
                    // Handle unexpected errors
                    res.code = 500;
                    res.body = string("Error retrieving data: ") + e.what();
                }

                return res;
            });

    // Definition of a route to apply project filters; takes a request parameter; returns success/failure response
    CROW_ROUTE(app, "/apply-filters").methods("POST"_method)
            ([&auth, &projectManager](const crow::request& req) {
                // Initialize response with CORS headers
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Extract and validate authentication token
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    // Parse and validate JSON request body
                    auto x = crow::json::load(req.body);
                    if (!x) {
                        res.code = 400;
                        res.body = "Invalid JSON";
                        return res;
                    }

                    // Apply individual filters if present in request
                    if (x.has("group")) projectManager.setFilter(FilterType::Group, x["group"].s());
                    if (x.has("projectType")) projectManager.setFilter(FilterType::ProjectType, x["projectType"].s());
                    if (x.has("billingPartner")) projectManager.setFilter(FilterType::BillingPartner, x["billingPartner"].s());
                    if (x.has("partner")) projectManager.setFilter(FilterType::Partner, x["partner"].s());
                    if (x.has("manager")) projectManager.setFilter(FilterType::Manager, x["manager"].s());
                    if (x.has("nextTask")) projectManager.setFilter(FilterType::NextTask, x["nextTask"].s());

                    // Apply date range filter if both dates are present
                    if (x.has("startDate") && x.has("endDate")) {
                        projectManager.setDateRangeFilter(x["startDate"].s(), x["endDate"].s());
                    }

                    // Apply extension status filter
                    if (x.has("extended")) {
                        projectManager.setFilter(FilterType::Extended, x["extended"].s());
                    }

                    // Apply report type filter
                    if (x.has("reportType")) {
                        projectManager.setFilter(FilterType::ReportType, x["reportType"].s());
                    }

                    // Set successful response
                    res.code = 200;
                    res.body = "Filters applied successfully";
                } catch (const exception& e) {
                    // Handle unexpected errors
                    res.code = 500;
                    res.body = string("Error applying filters: ") + e.what();
                }

                return res;
            });

    // Definition of a route to reset all project filters; takes a request parameter; returns success/failure response
    CROW_ROUTE(app, "/reset-filters").methods("POST"_method)
            ([&auth, &projectManager](const crow::request& req) {
                // Initialize response with CORS headers
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Extract and validate authentication token
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    // Reset all filters to default state
                    projectManager.resetFilter();

                    // Set successful response
                    res.code = 200;
                    res.body = "Filters reset successfully";
                } catch (const exception& e) {
                    // Handle unexpected errors
                    res.code = 500;
                    res.body = string("Error resetting filters: ") + e.what();
                }

                return res;
            });

    // Definition of a route to search projects by term; takes a request parameter; returns JSON array of matching projects
    CROW_ROUTE(app, "/search-projects").methods("GET"_method)
            ([&auth, &projectManager](const crow::request& req) {
                // Initialize response with CORS headers
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Extract and validate authentication token
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    // Get search term from URL parameters
                    string searchTerm = req.url_params.get("term") ? req.url_params.get("term") : "";

                    // Search projects and prepare response
                    vector<Project> projects = projectManager.searchProjects(searchTerm);
                    crow::json::wvalue response_body;
                    int i = 0;

                    // Convert each matching project to JSON format
                    for (const auto& project : projects) {
                        crow::json::wvalue projectJson;
                        projectJson["id"] = project.getId();
                        projectJson["group"] = project.getGroup();
                        projectJson["client"] = project.getClient();
                        projectJson["projectType"] = project.getProjectType();
                        projectJson["billingPartner"] = project.getBillingPartner();
                        projectJson["partner"] = project.getPartner();
                        projectJson["manager"] = project.getManager();
                        projectJson["nextTask"] = project.getNextTask();
                        projectJson["memo"] = project.getMemo();
                        projectJson["regularDeadline"] = project.getRegularDeadline().getDateStr();
                        projectJson["internalDeadline"] = project.getInternalDeadline().getDateStr();

                        // Add project to response array
                        response_body[i++] = move(projectJson);
                    }

                    // Set successful response with JSON data
                    res.body = response_body.dump();
                    res.code = 200;
                    res.add_header("Content-Type", "application/json");
                } catch (const exception& e) {
                    // Handle unexpected errors
                    res.code = 500;
                    res.body = string("Error searching projects: ") + e.what();
                }

                return res;
            });

    // Definition of a route to get project statistics; takes a request parameter; returns JSON with statistics
    CROW_ROUTE(app, "/statistics").methods("GET"_method)
            ([&auth, &projectManager](const crow::request& req) {
                // Initialize response with CORS headers
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Extract and validate authentication token
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    // Extract filter parameters from URL
                    string group = req.url_params.get("group") ? req.url_params.get("group") : "";
                    string projectType = req.url_params.get("projectType") ? req.url_params.get("projectType") : "";
                    string manager = req.url_params.get("manager") ? req.url_params.get("manager") : "";
                    string startDate = req.url_params.get("startDate") ? req.url_params.get("startDate") : "";
                    string endDate = req.url_params.get("endDate") ? req.url_params.get("endDate") : "";

                    // Retrieve and filter projects based on parameters
                    vector<Project> projects = projectManager.getAllProjects();
                    projects.erase(std::remove_if(projects.begin(), projects.end(),
                                                  [&](const Project& p) {
                                                      return (!group.empty() && p.getGroup() != group) ||
                                                             (!projectType.empty() && p.getProjectType() != projectType) ||
                                                             (!manager.empty() && p.getManager() != manager) ||
                                                             (!startDate.empty() && p.getRegularDeadline() < Date(startDate)) ||
                                                             (!endDate.empty() && p.getRegularDeadline() > Date(endDate));
                                                  }), projects.end());

                    // Initialize statistics counters
                    int totalProjects = projects.size();
                    int notFiled = 0;
                    int notReviewed = 0;
                    int awaitingCorrections = 0;
                    int awaitingEFileAuth = 0;
                    int unextended = 0;
                    int extended = 0;

                    // Calculate statistics based on project conditions
                    for (const auto& project : projects) {
                        if (ReportConditions::isNotFiled(project)) notFiled++;
                        if (ReportConditions::isNotReviewed(project)) notReviewed++;
                        if (ReportConditions::isAwaitingCorrections(project)) awaitingCorrections++;
                        if (ReportConditions::isAwaitingEFileAuthorization(project)) awaitingEFileAuth++;
                        if (ReportConditions::isUnextended(project)) unextended++;
                        if (project.isExtended()) extended++;
                    }

                    // Build JSON response with statistics
                    crow::json::wvalue response_body;
                    response_body["totalProjects"] = totalProjects;
                    response_body["notFiled"] = notFiled;
                    response_body["notReviewed"] = notReviewed;
                    response_body["awaitingCorrections"] = awaitingCorrections;
                    response_body["awaitingEFileAuth"] = awaitingEFileAuth;
                    response_body["unextended"] = unextended;
                    response_body["extended"] = extended;

                    // Set successful response with JSON data
                    res.body = response_body.dump();
                    res.code = 200;
                    res.add_header("Content-Type", "application/json");
                } catch (const exception& e) {
                    // Handle unexpected errors
                    res.code = 500;
                    res.body = string("Error retrieving statistics: ") + e.what();
                }

                return res;
            });

    // Definition of a route to handle user logout; takes a request parameter; returns success/failure response
    CROW_ROUTE(app, "/logout").methods("POST"_method)
            ([&auth](const crow::request& req) {
                // Initialize response with CORS headers
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Extract token from authorization header
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }

                    // Invalidate the user's token
                    auth.invalidateToken(token);

                    // Set successful response
                    res.code = 200;
                    res.body = "Logged out successfully";
                } catch (const exception& e) {
                    // Handle unexpected errors
                    res.code = 500;
                    res.body = string("Error during logout: ") + e.what();
                }

                return res;
            });

    // Definition of a route to serve the settings page; takes a request parameter; returns HTML response
    CROW_ROUTE(app, "/settings.html")([](const crow::request& req) {
        // Initialize response with CORS headers and content type
        crow::response res;
        addCorsHeaders(res);
        res.set_header("Content-Type", "text/html");

        try {
            // Attempt to open settings HTML file
            ifstream file("templates/settings.html");
            if (!file.is_open()) {
                // Return 404 if file not found
                res.code = 404;
                res.body = "Settings HTML file not found";
                return res;
            }

            // Read file contents and set successful response
            stringstream buffer;
            buffer << file.rdbuf();
            res.code = 200;
            res.body = buffer.str();
        } catch (const exception& e) {
            // Handle unexpected errors
            res.code = 500;
            res.body = string("Error reading settings file: ") + e.what();
        }

        return res;
    });

    // Definition of a route to handle password changes; takes a request parameter; returns JSON response
    CROW_ROUTE(app, "/change-password").methods("POST"_method)
            ([&auth](const crow::request& req) {
                // Initialize response with CORS headers
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Extract and validate authorization token
                    string token = req.get_header_value("Authorization");
                    if (token.empty()) {
                        res.code = 401;
                        res.body = crow::json::wvalue({{"success", false}, {"message", "No authorization token provided"}}).dump();
                        return res;
                    }

                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }

                    // Verify token validity
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = crow::json::wvalue({{"success", false}, {"message", "Invalid or expired token"}}).dump();
                        return res;
                    }

                    // Parse and validate request body
                    auto x = crow::json::load(req.body);
                    if (!x || !x.has("currentPassword") || !x.has("newPassword")) {
                        res.code = 400;
                        res.body = crow::json::wvalue({{"success", false}, {"message", "Missing required fields"}}).dump();
                        return res;
                    }

                    // Extract password values
                    string currentPassword = x["currentPassword"].s();
                    string newPassword = x["newPassword"].s();

                    // Process password change request
                    try {
                        User user = auth.getUserFromToken(token);
                        bool success = auth.changePassword(user.getUsername(), currentPassword, newPassword);

                        if (success) {
                            // Return success response
                            res.code = 200;
                            res.body = crow::json::wvalue({{"success", true}, {"message", "Password changed successfully"}}).dump();
                        } else {
                            // Return failure response with user-friendly message
                            res.code = 400;
                            res.body = crow::json::wvalue({
                                                                  {"success", false},
                                                                  {"message", "Password change failed. Please check your current password and ensure enough time has passed since your last change."}
                                                          }).dump();
                        }
                    } catch (const exception& e) {
                        // Handle password change processing errors
                        res.code = 500;
                        res.body = crow::json::wvalue({
                                                              {"success", false},
                                                              {"message", "Error processing password change request"}
                                                      }).dump();
                        return res;
                    }

                } catch (const exception& e) {
                    // Handle unexpected errors
                    res.code = 500;
                    res.body = crow::json::wvalue({
                                                          {"success", false},
                                                          {"message", "Internal server error during password change"}
                                                  }).dump();
                }

                // Set JSON content type and return response
                res.add_header("Content-Type", "application/json");
                return res;
            });

}