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

void setupRoutes (crow::SimpleApp& app, Auth& auth, ReminderSystem& reminderSystem, ProjectManager& projectManager, LacerteCrossReference& lacerteCrossRef, ProjectsDatabase& projectsDatabase)  {

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

    // Definition of a route to process Lacerte cross-reference; takes a request parameter with file content; returns JSON response with match metrics
    CROW_ROUTE(app, "/cross-reference-lacerte").methods("POST"_method)
    ([&auth, &projectManager, &lacerteCrossRef](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);

        try {
            cout << "\n=== Starting Lacerte Cross-Reference Process ===" << endl << flush;

            // 1. Validate token
            cout << "Validating authentication token..." << endl << flush;
            string token = req.get_header_value("Authorization");
            if (token.substr(0, 7) == "Bearer ") token = token.substr(7);
            if (!auth.validateToken(token)) {
                cout << "Authentication failed - invalid token" << endl << flush;
                res.code = 401;
                res.body = "Invalid token";
                return res;
            }
            cout << "Authentication successful" << endl << flush;

            // 2. Get projects and precompute features
            cout << "Retrieving projects from database..." << endl << flush;
            vector<Project> projects = projectManager.getAllProjects();
            cout << "Retrieved " << projects.size() << " projects" << endl << flush;

            cout << "Precomputing database features..." << endl << flush;
            auto precomputedFeatures = lacerteCrossRef.precomputeDatabaseFeatures(projects);
            cout << "Features precomputed for " << precomputedFeatures.size() << " entries" << endl << flush;

            // 3. Parse uploaded file content
            cout << "Parsing uploaded file content..." << endl << flush;
            auto x = crow::json::load(req.body);
            if (!x || !x.has("fileContent")) {
                cout << "Error: Missing file content in request" << endl << flush;
                res.code = 400;
                res.body = "Missing file content";
                return res;
            }

            // 4. Process CSV content
            string fileContent = x["fileContent"].s();
            istringstream stream(fileContent);
            string line;

            cout << "Beginning CSV processing..." << endl << flush;
            vector<MatchResult> results;
            int totalProcessed = 0;
            int matchesFound = 0;

            // Count total lines for progress calculation
            int totalLines = count(fileContent.begin(), fileContent.end(), '\n') - 1; // Subtract 1 for header
            cout << "Total lines to process: " << totalLines << endl << flush;

            // Skip header line
            getline(stream, line);

            // 5. Process each line using precomputed features
            while (getline(stream, line)) {
                totalProcessed++;
                if (totalProcessed % 50 == 0) {
                    // Create progress JSON
                    crow::json::wvalue progress;
                    progress["processed"] = totalProcessed;
                    progress["total"] = totalLines;
                    progress["percentage"] = (totalProcessed * 100.0 / totalLines);

                    cout << "Progress: " << totalProcessed << " entries processed ("
                         << fixed << setprecision(1)
                         << (totalProcessed * 100.0 / totalLines) << "%)" << endl << flush;
                }

                istringstream lineStream(line);
                string lacerteName;
                getline(lineStream, lacerteName, ',');

                if (lacerteName.empty()) continue;

                MatchResult currentMatch;
                currentMatch.lacerteName = lacerteName;
                currentMatch.confidence = 0.0;
                currentMatch.databaseMatch = "";

                // Track best match regardless of confidence threshold
                string bestMatch = "";
                double bestConfidence = 0.0;

                // Compare against precomputed database entries
                for (const auto& precomputed : precomputedFeatures) {
                    double confidence = lacerteCrossRef.getMatchConfidence(lacerteName, precomputed.clientName);

                    if (confidence > bestConfidence) {
                        bestConfidence = confidence;
                        bestMatch = precomputed.clientName;
                    }
                }

                // Determine if it's a match based on threshold
                if (bestConfidence > 0.7) {
                    currentMatch.confidence = bestConfidence;
                    currentMatch.databaseMatch = bestMatch;
                    matchesFound++;
                } else {
                    currentMatch.confidence = bestConfidence;
                    currentMatch.databaseMatch = "Closest: " + bestMatch;
                }

                results.push_back(currentMatch);
            }

            // 6. Write results to CSV
            cout << "\nCSV processing completed:" << endl << flush;
            cout << "Total entries processed: " << totalProcessed << endl << flush;
            cout << "Matches found: " << matchesFound << endl << flush;
            cout << "Writing results to CSV file..." << endl << flush;

            ofstream outFile("cross_reference_results.csv");
            outFile << "Lacerte Name,Database Match,Confidence Score,Notes\n";

            for (const auto& match : results) {
                outFile << "\"" << match.lacerteName << "\",";

                if (match.confidence > 0.7) {
                    outFile << "\"" << match.databaseMatch << "\",";
                } else {
                    outFile << "\"No Match\",";
                }

                outFile << fixed << setprecision(4) << match.confidence << ",";

                if (match.confidence <= 0.7) {
                    outFile << "\"Closest match: " << match.databaseMatch << "\"";
                }

                outFile << "\n";
            }

            outFile.close();
            cout << "Results file written successfully" << endl << flush;

            // 7. Prepare response with metrics
            auto metrics = lacerteCrossRef.getModelMetrics();

            crow::json::wvalue response;
            response["success"] = true;
            response["message"] = "Cross reference completed";
            response["totalProcessed"] = totalProcessed;
            response["matchesFound"] = matchesFound;
            response["metrics"]["accuracy"] = metrics.getAccuracyRate();
            response["metrics"]["matchRate"] = metrics.getMatchRate();
            response["metrics"]["totalPredictions"] = metrics.totalPredictions;
            response["metrics"]["correctMatches"] = metrics.correct_matches;
            response["resultsFile"] = "cross_reference_results.csv";

            res.code = 200;
            res.body = response.dump();

            cout << "=== Cross-Reference Process Completed Successfully ===\n" << endl;

        } catch (const exception& e) {
            cout << "ERROR: Cross-reference process failed: " << e.what() << endl << flush;
            res.code = 500;
            res.body = string("Error processing Lacerte file: ") + e.what();
        }
        return res;
    });

    // Definition of a route to stream cross-reference progress; takes a request parameter; returns server-sent event stream response
    CROW_ROUTE(app, "/cross-reference-progress")
            ([](const crow::request& req) {
                crow::response res;
                res.add_header("Content-Type", "text/event-stream");
                res.add_header("Cache-Control", "no-cache");
                res.add_header("Connection", "keep-alive");
                return res;
            });

    // Definition of a route to complete feedback session and retrain model; takes a request parameter; returns JSON response with metrics
    CROW_ROUTE(app, "/complete-feedback-session")
            ([&lacerteCrossRef](const crow::request& req) {
                try {

                    // Force retrain without storing dummy data
                    lacerteCrossRef.updateModelWithFeedback(
                            "",     // empty name1
                            "",     // empty name2
                            true,   // dummy match
                            1.0,    // dummy confidence
                            true    // force retrain
                    );

                    auto metrics = lacerteCrossRef.getModelMetrics();

                    crow::json::wvalue response;
                    response["success"] = true;
                    response["message"] = "Feedback session completed and model retrained";
                    response["metrics"]["accuracy"] = metrics.accuracy;
                    response["metrics"]["total_predictions"] = metrics.totalPredictions;
                    response["metrics"]["correct_matches"] = metrics.correct_matches;

                    return crow::response(200, response.dump());
                } catch (const exception& e) {
                    return crow::response(500, string("Error completing feedback session: ") + e.what());
                }
            });

    // Definition of a route to serve cross-reference page; takes a request parameter; returns HTML response
    CROW_ROUTE(app, "/cross-reference.html")([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        res.set_header("Content-Type", "text/html");

        try {
            std::ifstream file("templates/cross-reference.html");
            if (!file.is_open()) {
                res.code = 404;
                res.body = "Cross Reference HTML file not found";
                return res;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            res.code = 200;
            res.body = buffer.str();
        } catch (const std::exception& e) {
            res.code = 500;
            res.body = std::string("Error reading cross-reference file: ") + e.what();
        }

        return res;
    });

    // Definition of a route to download cross-reference results; takes a request parameter; returns CSV file response
    CROW_ROUTE(app, "/cross_reference_results.csv").methods("GET"_method)
            ([](const crow::request& req) {
                crow::response res;

                // Read the CSV file
                std::ifstream file("cross_reference_results.csv");
                if (!file) {
                    res.code = 404;
                    res.body = "Results file not found";
                    return res;
                }

                std::stringstream buffer;
                buffer << file.rdbuf();

                // Set appropriate headers
                res.set_header("Content-Type", "text/csv");
                res.set_header("Content-Disposition", "attachment; filename=cross_reference_results.csv");
                res.body = buffer.str();

                return res;
            });

    // Definition of a route to handle model feedback; takes a request parameter with match data; returns JSON response
    CROW_ROUTE(app, "/api/feedback").methods("POST"_method)
            ([&lacerteCrossRef](const crow::request& req) {
                auto body = crow::json::load(req.body);

                // Validate JSON body
                if (!body ||
                    !body.has("name1") ||
                    !body.has("name2") ||
                    !body.has("isMatch")) {
                    return crow::response(400, crow::json::wvalue({
                                                                          {"success", false},
                                                                          {"message", "Invalid request body"}
                                                                  }));
                }

                try {
                    string name1 = body["name1"].s();
                    string name2 = body["name2"].s();
                    bool isMatch = body["isMatch"].b();
                    double confidence = lacerteCrossRef.getMatchConfidence(name1, name2);

                    lacerteCrossRef.updateModelWithFeedback(name1, name2, isMatch, confidence);

                    return crow::response(200, crow::json::wvalue({
                                                                          {"success", true},
                                                                          {"message", "Feedback recorded successfully"}
                                                                  }));
                } catch (const exception& e) {
                    return crow::response(500, crow::json::wvalue({
                                                                          {"success", false},
                                                                          {"message", e.what()}
                                                                  }));
                }
            });

    CROW_ROUTE(app, "/model-stats")
            ([&auth, &lacerteCrossRef](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Validate token
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") token = token.substr(7);
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    auto metrics = lacerteCrossRef.getModelMetrics();

                    // Calculate batch accuracy (for current upload)
                    double batchAccuracy = metrics.matches_found > 0 ?
                                           (double)(metrics.matches_found + metrics.correct_matches) /
                                           (metrics.matches_found + metrics.total_high_confidence) : 0.0;

                    // Add debug output

                    crow::json::wvalue response{
                        {"accuracy", metrics.accuracy},
                        {"batchAccuracy", batchAccuracy},  // Add batch accuracy
                        {"totalMatches", metrics.matches_found},
                        {"learningProgress", metrics.totalPredictions},
                        {"correctMatches", metrics.correct_matches}
                    };

                    res.code = 200;
                    res.body = response.dump();
                    return res;

                } catch (const exception& e) {
                    res.code = 500;
                    res.body = string("Error getting model stats: ") + e.what();
                    return res;
                }
            });

    CROW_ROUTE(app, "/statistics/awaiting-efile-auth").methods("GET"_method)
            ([&auth, &projectManager](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    // Validate token
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    // Get filter parameters from query string
                    string manager = req.url_params.get("manager") ? req.url_params.get("manager") : "";
                    string group = req.url_params.get("group") ? req.url_params.get("group") : "";
                    string projectType = req.url_params.get("projectType") ? req.url_params.get("projectType") : "";

                    // Get all projects and filter them
                    vector<Project> projects = projectManager.getAllProjects();
                    vector<crow::json::wvalue> awaitingEFileAuth;

                    for (const auto& project : projects) {
                        // Apply filters
                        if ((!manager.empty() && project.getManager() != manager) ||
                            (!group.empty() && project.getGroup() != group) ||
                            (!projectType.empty() && project.getProjectType() != projectType)) {
                            continue;
                        }

                        // Check if project is awaiting e-file authorization
                        if (project.getNextTask() == "E-file Sent to Client") {
                            crow::json::wvalue projectJson;
                            projectJson["id"] = project.getId();
                            projectJson["client"] = project.getClient();
                            projectJson["manager"] = project.getManager();
                            projectJson["deadline"] = project.getRegularDeadline().getDateStr();
                            projectJson["projectType"] = project.getProjectType();
                            awaitingEFileAuth.push_back(std::move(projectJson));
                        }
                    }

                    res.code = 200;
                    res.body = crow::json::wvalue(awaitingEFileAuth).dump();
                    res.add_header("Content-Type", "application/json");

                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = std::string("Error retrieving awaiting e-file auth: ") + e.what();
                }

                return res;
            });                        
}