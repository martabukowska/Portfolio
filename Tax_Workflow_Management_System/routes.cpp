#include "routes.h"
#include "report_generator.h"
#include "statistics.h"
#include "crow/mustache.h"
#include "Lacerte_cross_ref.h"
#include <chrono>
#include <thread>

using namespace std;
using namespace TaxReturnSystem;

void addCorsHeaders(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

void setupRoutes(crow::SimpleApp& app, Auth& auth, ReminderSystem& reminderSystem, ProjectManager& projectManager, LacerteCrossReference& lacerteCrossRef, ProjectsDatabase& projectsDatabase) {

    CROW_ROUTE(app, "/<path>").methods("OPTIONS"_method)
            ([](const crow::request& req, crow::response& res, string path) {
                addCorsHeaders(res);
                res.code = 204;
                res.end();
            });

    CROW_ROUTE(app, "/")([]() {
        crow::response res;
        addCorsHeaders(res);

        res.add_header("Cache-Control", "no-cache, no-store, must-revalidate");
        res.add_header("Pragma", "no-cache");
        res.add_header("Expires", "0");

        std::ifstream file("templates/index.html");
        if (file) {
            std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            res.body = contents;
            res.code = 200;
            res.set_header("Content-Type", "text/html");
        } else {
            res.body = "File not found";
            res.code = 404;
        }
        return res;
    });

    // Dashboard route
    CROW_ROUTE(app, "/dashboard.html")([](const crow::request& req) {
        crow::response res;
        res.code = 200;
        res.set_header("Content-Type", "text/html");
        addCorsHeaders(res);

        // Read the HTML file
        std::ifstream file("templates/dashboard.html");
        if (file) {
            std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            res.body = contents;
        } else {
            res.body = "Dashboard HTML file not found";
            res.code = 404;
        }

        return res;
    });

    CROW_ROUTE(app, "/filter-options").methods("GET"_method)
            ([&auth, &projectManager](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    vector<Project> projects = projectManager.getAllProjects();

                    set<string> groups;
                    set<string> projectTypes;
                    set<string> managers;
                    set<string> partners;

                    for (const auto& project : projects) {
                        if (!project.getGroup().empty()) groups.insert(project.getGroup());
                        if (!project.getProjectType().empty()) projectTypes.insert(project.getProjectType());
                        if (!project.getManager().empty()) managers.insert(project.getManager());
                        if (!project.getPartner().empty()) {
                            partners.insert(project.getPartner());
                        }
                    }

                    for (const auto& partner : partners) {
                    }

                    crow::json::wvalue response_body;
                    response_body["groups"] = vector<string>(groups.begin(), groups.end());
                    response_body["projectTypes"] = vector<string>(projectTypes.begin(), projectTypes.end());
                    response_body["managers"] = vector<string>(managers.begin(), managers.end());
                    response_body["partners"] = vector<string>(partners.begin(), partners.end());

                    res.body = response_body.dump();
                    res.code = 200;
                    res.add_header("Content-Type", "application/json");
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = std::string("Error retrieving filter options: ") + e.what();
                }

                return res;
            });

    // Login route
    CROW_ROUTE(app, "/login").methods("POST"_method)
            ([&auth](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    auto x = crow::json::load(req.body);
                    if (!x) {
                        res.code = 400;
                        res.body = "Invalid JSON";
                        return res;
                    }

                    string username = x["username"].s();
                    string password = x["password"].s();

                    LoginResult result = auth.loginUser(username, password);

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
                            res.code = 200;
                            crow::json::wvalue response_body;
                            response_body["token"] = auth.createTokenForUser(username);
                            res.body = response_body.dump();
                            res.add_header("Content-Type", "application/json");
                            break;
                    }
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = std::string("Internal server error: ") + e.what();
                }

                return res;
            });

    // Registration route
    CROW_ROUTE(app, "/register").methods("POST"_method)
            ([&auth](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    auto x = crow::json::load(req.body);
                    if (!x) {
                        res.code = 400;
                        res.body = crow::json::wvalue({{"success", false}, {"message", "Invalid JSON"}}).dump();
                        return res;
                    }

                    string username = x["username"].s();
                    string email = x["email"].s();
                    string password = x["password"].s();

                    if (auth.registerUser(username, password, email)) {
                        res.code = 201;
                        res.body = crow::json::wvalue({{"success", true}, {"message", "User registered successfully"}}).dump();
                    } else {
                        res.code = 400;
                        res.body = crow::json::wvalue({{"success", false}, {"message", "Registration failed"}}).dump();
                    }
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = crow::json::wvalue({{"success", false}, {"message", std::string("Internal server error: ") + e.what()}}).dump();
                }

                res.add_header("Content-Type", "application/json");
                return res;
            });

    CROW_ROUTE(app, "/reset-password-request").methods("POST"_method)
            ([&auth](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    auto x = crow::json::load(req.body);
                    if (!x) {
                        res.code = 400;
                        res.body = crow::json::wvalue({{"success", false}, {"message", "Invalid JSON"}}).dump();
                        return res;
                    }

                    string email = x["email"].s();
                    bool result = auth.requestPasswordReset(email);

                    if (result) {
                        res.code = 200;
                        res.body = crow::json::wvalue({{"success", true}, {"message", "Password reset request successful. Check your email for further instructions."}}).dump();
                    } else {
                        res.code = 400;
                        res.body = crow::json::wvalue({{"success", false}, {"message", "Password reset request failed."}}).dump();
                    }
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = crow::json::wvalue({{"success", false}, {"message", std::string("Internal server error: ") + e.what()}}).dump();
                }

                res.add_header("Content-Type", "application/json");
                return res;
            });

    // Get user data route
    CROW_ROUTE(app, "/user").methods("GET"_method)
            ([&auth](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    User user = auth.getUserFromToken(token);
                    crow::json::wvalue response_body;
                    response_body["username"] = user.getUsername();
                    response_body["email"] = user.getEmail();
                    res.body = response_body.dump();
                    res.code = 200;
                    res.add_header("Content-Type", "application/json");
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = std::string("Internal server error: ") + e.what();
                }

                return res;
            });

    // Password reset confirmation route
    CROW_ROUTE(app, "/reset-password-confirm").methods("POST"_method)
            ([&auth](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    auto x = crow::json::load(req.body);
                    if (!x || !x.has("email") || !x.has("token") || !x.has("newPassword")) {
                        res.code = 400;
                        res.body = "Invalid JSON or missing required fields";
                        return res;
                    }

                    string email = x["email"].s();
                    string token = x["token"].s();
                    string newPassword = x["newPassword"].s();

                    bool result = auth.resetPassword(email, token, newPassword);

                    if (result) {
                        res.code = 200;
                        res.body = "Password reset successful.";
                    } else {
                        res.code = 400;
                        res.body = "Password reset failed.";
                    }
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = std::string("Internal server error: ") + e.what();
                }

                return res;
            });

    // Route to update user's email
    CROW_ROUTE(app, "/user/update-email").methods("POST"_method)
            ([&auth](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") token = token.substr(7);
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    auto x = crow::json::load(req.body);
                    if (!x || !x.has("newEmail")) {
                        res.code = 400;
                        res.body = "Invalid JSON or missing parameters";
                        return res;
                    }

                    string newEmail = x["newEmail"].s();

                    User user = auth.getUserFromToken(token);
                    user.setEmail(newEmail);

                    if (user.getEmail() == newEmail) {
                        res.code = 200;
                        res.body = "User email updated successfully";
                    } else {
                        res.code = 400;
                        res.body = "Failed to update user email";
                    }
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = std::string("Internal server error: ") + e.what();
                }

                return res;
            });

    // Admin-only route to load CSV data
    CROW_ROUTE(app, "/admin/load-csv").methods("POST"_method)
            ([&auth, &projectManager](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {

                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    User user = auth.getUserFromToken(token);
                    if (user.getRole() != UserRole::Admin) {
                        res.code = 403;
                        res.body = "Unauthorized access";
                        return res;
                    }

                    string contentType = req.get_header_value("Content-Type");
                    ReportType reportType = ReportType::RegularDeadline;
                    string filename;

                    if (contentType.find("application/json") != string::npos) {
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

                        bool success = projectManager.importFromCSV(filename, reportType);

                        if (!success) {
                            res.code = 500;
                            res.body = "Failed to import CSV data";
                            return res;
                        }

                        // Get count of imported projects
                        vector<Project> projects = projectManager.getAllProjects();

                        res.code = 200;
                        res.body = "CSV data loaded successfully. Imported " + to_string(projects.size()) + " projects.";
                    } else {
                        res.code = 400;
                        res.body = "Unsupported Content-Type";
                    }
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = std::string("Error loading CSV data: ") + e.what();
                }

                return res;
            });

    // Route to get all data
    CROW_ROUTE(app, "/data").methods("GET"_method)
            ([&auth, &projectManager](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    vector<Project> projects = projectManager.getAllProjects();
                    crow::json::wvalue response_body;
                    int i = 0;
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
                        response_body[i++] = std::move(projectJson);
                    }
                    res.body = response_body.dump();
                    res.code = 200;
                    res.add_header("Content-Type", "application/json");
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = std::string("Error retrieving data: ") + e.what();
                }

                return res;
            });

    // Route to apply filters
    CROW_ROUTE(app, "/apply-filters").methods("POST"_method)
            ([&auth, &projectManager](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    auto x = crow::json::load(req.body);
                    if (!x) {
                        res.code = 400;
                        res.body = "Invalid JSON";
                        return res;
                    }

                    if (x.has("group")) projectManager.setFilter(FilterType::Group, x["group"].s());
                    if (x.has("projectType")) projectManager.setFilter(FilterType::ProjectType, x["projectType"].s());
                    if (x.has("billingPartner")) projectManager.setFilter(FilterType::BillingPartner, x["billingPartner"].s());
                    if (x.has("partner")) projectManager.setFilter(FilterType::Partner, x["partner"].s());
                    if (x.has("manager")) projectManager.setFilter(FilterType::Manager, x["manager"].s());
                    if (x.has("nextTask")) projectManager.setFilter(FilterType::NextTask, x["nextTask"].s());
                    if (x.has("startDate") && x.has("endDate")) {
                        projectManager.setDateRangeFilter(x["startDate"].s(), x["endDate"].s());
                    }
                    if (x.has("extended")) {
                        projectManager.setFilter(FilterType::Extended, x["extended"].s());
                    }
                    if (x.has("reportType")) {
                        projectManager.setFilter(FilterType::ReportType, x["reportType"].s());
                    }

                    res.code = 200;
                    res.body = "Filters applied successfully";
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = std::string("Error applying filters: ") + e.what();
                }

                return res;
            });

    // Route to reset filters
    CROW_ROUTE(app, "/reset-filters").methods("POST"_method)
            ([&auth, &projectManager](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    projectManager.resetFilter();
                    res.code = 200;
                    res.body = "Filters reset successfully";
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = std::string("Error resetting filters: ") + e.what();
                }

                return res;
            });

    // Route to search projects
    CROW_ROUTE(app, "/search-projects").methods("GET"_method)
            ([&auth, &projectManager](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    string searchTerm = req.url_params.get("term") ? req.url_params.get("term") : "";
                    vector<Project> projects = projectManager.searchProjects(searchTerm);
                    crow::json::wvalue response_body;
                    int i = 0;
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
                        response_body[i++] = std::move(projectJson);
                    }
                    res.body = response_body.dump();
                    res.code = 200;
                    res.add_header("Content-Type", "application/json");
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = std::string("Error searching projects: ") + e.what();
                }

                return res;
            });

    CROW_ROUTE(app, "/statistics").methods("GET"_method)
            ([&auth, &projectManager](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    string group = req.url_params.get("group") ? req.url_params.get("group") : "";
                    string projectType = req.url_params.get("projectType") ? req.url_params.get("projectType") : "";
                    string manager = req.url_params.get("manager") ? req.url_params.get("manager") : "";
                    string startDate = req.url_params.get("startDate") ? req.url_params.get("startDate") : "";
                    string endDate = req.url_params.get("endDate") ? req.url_params.get("endDate") : "";

                    vector<Project> projects = projectManager.getAllProjects();

                    projects.erase(std::remove_if(projects.begin(), projects.end(),
                                                  [&](const Project& p) {
                                                      return (!group.empty() && p.getGroup() != group) ||
                                                             (!projectType.empty() && p.getProjectType() != projectType) ||
                                                             (!manager.empty() && p.getManager() != manager) ||
                                                             (!startDate.empty() && p.getRegularDeadline() < Date(startDate)) ||
                                                             (!endDate.empty() && p.getRegularDeadline() > Date(endDate));
                                                  }), projects.end());

                    int totalProjects = projects.size();
                    int notFiled = 0;
                    int notReviewed = 0;
                    int awaitingCorrections = 0;
                    int awaitingEFileAuth = 0;
                    int unextended = 0;
                    int extended = 0;

                    for (const auto& project : projects) {
                        if (ReportConditions::isNotFiled(project)) notFiled++;
                        if (ReportConditions::isNotReviewed(project)) notReviewed++;
                        if (ReportConditions::isAwaitingCorrections(project)) awaitingCorrections++;
                        if (ReportConditions::isAwaitingEFileAuthorization(project)) awaitingEFileAuth++;
                        if (ReportConditions::isUnextended(project)) unextended++;
                        if (project.isExtended()) extended++;
                    }

                    crow::json::wvalue response_body;
                    response_body["totalProjects"] = totalProjects;
                    response_body["notFiled"] = notFiled;
                    response_body["notReviewed"] = notReviewed;
                    response_body["awaitingCorrections"] = awaitingCorrections;
                    response_body["awaitingEFileAuth"] = awaitingEFileAuth;
                    response_body["unextended"] = unextended;
                    response_body["extended"] = extended;

                    res.body = response_body.dump();
                    res.code = 200;
                    res.add_header("Content-Type", "application/json");
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = std::string("Error retrieving statistics: ") + e.what();
                }

                return res;
            });

    CROW_ROUTE(app, "/logout").methods("POST"_method)
            ([&auth](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }

                    auth.invalidateToken(token);

                    res.code = 200;
                    res.body = "Logged out successfully";
                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = std::string("Error during logout: ") + e.what();
                }

                return res;
            });

    CROW_ROUTE(app, "/settings.html")([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        res.set_header("Content-Type", "text/html");

        try {
            std::ifstream file("templates/settings.html");
            if (!file.is_open()) {
                res.code = 404;
                res.body = "Settings HTML file not found";
                return res;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            res.code = 200;
            res.body = buffer.str();
        } catch (const std::exception& e) {
            res.code = 500;
            res.body = std::string("Error reading settings file: ") + e.what();
        }

        return res;
    });

    CROW_ROUTE(app, "/change-password").methods("POST"_method)
            ([&auth](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    string token = req.get_header_value("Authorization");
                    if (token.empty()) {
                        res.code = 401;
                        res.body = crow::json::wvalue({{"success", false}, {"message", "No authorization token provided"}}).dump();
                        return res;
                    }

                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }

                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = crow::json::wvalue({{"success", false}, {"message", "Invalid or expired token"}}).dump();
                        return res;
                    }

                    auto x = crow::json::load(req.body);
                    if (!x || !x.has("currentPassword") || !x.has("newPassword")) {
                        res.code = 400;
                        res.body = crow::json::wvalue({{"success", false}, {"message", "Missing required fields"}}).dump();
                        return res;
                    }

                    string currentPassword = x["currentPassword"].s();
                    string newPassword = x["newPassword"].s();

                    // Get user and change password
                    try {
                        User user = auth.getUserFromToken(token);
                        bool success = auth.changePassword(user.getUsername(), currentPassword, newPassword);

                        if (success) {
                            res.code = 200;
                            res.body = crow::json::wvalue({{"success", true}, {"message", "Password changed successfully"}}).dump();
                        } else {
                            res.code = 400;
                            res.body = crow::json::wvalue({
                                                                  {"success", false},
                                                                  {"message", "Password change failed. Please check your current password and ensure enough time has passed since your last change."}
                                                          }).dump();
                        }
                    } catch (const std::exception& e) {
                        res.code = 500;
                        res.body = crow::json::wvalue({
                                                              {"success", false},
                                                              {"message", "Error processing password change request"}
                                                      }).dump();
                        return res;
                    }

                } catch (const std::exception& e) {
                    res.code = 500;
                    res.body = crow::json::wvalue({
                                                          {"success", false},
                                                          {"message", "Internal server error during password change"}
                                                  }).dump();
                }

                res.add_header("Content-Type", "application/json");
                return res;
            });

    CROW_ROUTE(app, "/cross-reference-lacerte").methods("POST"_method)
            ([&auth, &projectManager, &lacerteCrossRef](const crow::request& req) {
                crow::response res;
                addCorsHeaders(res);

                try {
                    // 1. Validate token
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") token = token.substr(7);
                    if (!auth.validateToken(token)) {
                        cout << "Authentication failed - invalid token" << endl << flush;
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }
                    // 2. Get projects and precompute features
                    vector<Project> projects = projectManager.getAllProjects();

                    // 3-4. Parse uploaded file content and collect names
                    auto x = crow::json::load(req.body);
                    if (!x || !x.has("fileContent")) {
                        cout << "Error: Missing file content in request" << endl << flush;
                        res.code = 400;
                        res.body = "Missing file content";
                        return res;
                    }

                    string fileContent = x["fileContent"].s();
                    istringstream stream(fileContent);
                    string line;
                    vector<string> lacerteNames;

                    getline(stream, line); // Skip header
                    while (getline(stream, line)) {
                        istringstream lineStream(line);
                        string lacerteName;
                        getline(lineStream, lacerteName, ',');
                        if (!lacerteName.empty()) {
                            lacerteNames.push_back(lacerteName);
                        }
                    }

                    // 5. Precompute database features
                    auto startPrecompute = chrono::high_resolution_clock::now();
                    auto precomputedFeatures = lacerteCrossRef.precomputeDatabaseFeatures(projects);
                    auto endPrecompute = chrono::high_resolution_clock::now();
                    auto precomputeTime = chrono::duration_cast<chrono::milliseconds>(endPrecompute - startPrecompute);

                    // 6. Parallel matching process
                    cout << "DEBUG: Input validation:"
                         << "\n - lacerteNames size: " << lacerteNames.size()
                         << "\n - First few lacerte names: ";
                    for (size_t i = 0; i < min(size_t(3), lacerteNames.size()); i++) {
                        cout << "\n   " << i + 1 << ". " << lacerteNames[i];
                    }
                    cout << "\n - precomputedFeatures size: " << precomputedFeatures.size()
                         << "\n - First few precomputed features: ";
                    for (size_t i = 0; i < min(size_t(3), precomputedFeatures.size()); i++) {
                        cout << "\n   " << i + 1 << ". Client: " << precomputedFeatures[i].clientName;
                    }
                    cout << endl << flush;

                    auto startMatching = chrono::high_resolution_clock::now();

                    // Call findMatches
                    vector<MatchResult> results = lacerteCrossRef.findMatches(lacerteNames, precomputedFeatures);

                    auto endMatching = chrono::high_resolution_clock::now();
                    auto matchingTime = chrono::duration_cast<chrono::milliseconds>(endMatching - startMatching);
                    cout << "\nParallel matching completed in " << matchingTime.count() << "ms" << endl << flush;

                    // 7. Write results to CSV
                    ofstream outFile("cross_reference_results.csv");
                    outFile << "Lacerte Name,Database Match,Confidence Score,Notes\n";

                    int matchesFound = 0;
                    for (const auto& match : results) {
                        outFile << "\"" << match.lacerteName << "\",";
                        if (match.confidence > 0.7) {
                            outFile << "\"" << match.databaseMatch << "\",";
                            matchesFound++;
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

                    // 8. Prepare response
                    auto metrics = lacerteCrossRef.getModelMetrics();
                    crow::json::wvalue response;
                    response["success"] = true;
                    response["message"] = "Cross reference completed";
                    response["totalProcessed"] = lacerteNames.size();
                    response["matchesFound"] = matchesFound;
                    response["metrics"]["accuracy"] = metrics.getAccuracyRate();
                    response["metrics"]["matchRate"] = metrics.getMatchRate();
                    response["metrics"]["totalPredictions"] = metrics.totalPredictions;
                    response["metrics"]["correctMatches"] = metrics.correctMatches;
                    response["resultsFile"] = "cross_reference_results.csv";
                    response["processingTimeMs"] = matchingTime.count();

                    res.code = 200;
                    res.body = response.dump();

                    cout << "\n=== Cross-Reference Process Summary ===" << endl
                         << "Total names processed: " << lacerteNames.size() << endl
                         << "Matches found: " << matchesFound << endl
                         << "Processing time: " << matchingTime.count() << "ms" << endl
                         << "Average time per name: "
                         << (lacerteNames.size() > 0 ? matchingTime.count() / lacerteNames.size() : 0)
                         << "ms" << endl
                         << "=================================" << endl << flush;

                } catch (const exception& e) {
                    cout << "ERROR: Cross-reference process failed: " << e.what() << endl << flush;
                    res.code = 500;
                    res.body = string("Error processing Lacerte file: ") + e.what();
                }
                return res;
            });

    CROW_ROUTE(app, "/cross-reference-progress")
            ([](const crow::request& req) {
                crow::response res;
                res.add_header("Content-Type", "text/event-stream");
                res.add_header("Cache-Control", "no-cache");
                res.add_header("Connection", "keep-alive");
                return res;
            });

    CROW_ROUTE(app, "/test-db-tables")
            ([&projectsDatabase](const crow::request& req) {
                try {
                    projectsDatabase.createTablesIfNotExist();
                    return crow::response(200, "Database table check completed");
                } catch (const exception& e) {
                    return crow::response(500, string("Database error: ") + e.what());
                }
            });

    CROW_ROUTE(app, "/test-feedback-simple")
            ([&lacerteCrossRef](const crow::request& req) {
                try {

                    string testName1 = "Test Company ABC";
                    string testName2 = "Test Company XYZ";
                    bool isMatch = true;
                    double confidence = 0.85;

                    lacerteCrossRef.updateModelWithFeedback(testName1, testName2, isMatch, confidence);

                    return crow::response(200, "Test feedback forced successfully");
                } catch (const exception& e) {
                    return crow::response(500, string("Error: ") + e.what());
                }
            });

    CROW_ROUTE(app, "/test-direct-feedback")
            ([&projectsDatabase](const crow::request& req) {
                try {

                    bool stored = projectsDatabase.storeFeedback(
                            "Test Direct Company ABC",  // lacerte_name
                            "Test Direct Company XYZ",  // database_name
                            true,                       // is_match
                            0.85                        // confidence
                    );

                    if (stored) {
                        return crow::response(200, "Direct feedback storage successful");
                    } else {
                        return crow::response(500, "Failed to store feedback directly");
                    }
                } catch (const exception& e) {
                    return crow::response(500, string("Direct feedback error: ") + e.what());
                }
            });

    CROW_ROUTE(app, "/test-lacerte-db")
            ([&lacerteCrossRef](const crow::request& req) {
                try {

                    bool success = lacerteCrossRef.testDatabaseAccess();

                    if (success) {
                        return crow::response(200, "LacerteCrossReference database access successful");
                    } else {
                        return crow::response(500, "Failed to access database through LacerteCrossReference");
                    }
                } catch (const exception& e) {
                    return crow::response(500, string("LacerteCrossReference test error: ") + e.what());
                }
            });

    CROW_ROUTE(app, "/complete-feedback-session")
            ([&lacerteCrossRef](const crow::request& req) {
                try {
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
                    response["metrics"]["correct_matches"] = metrics.correctMatches;

                    return crow::response(200, response.dump());
                } catch (const exception& e) {
                    return crow::response(500, string("Error completing feedback session: ") + e.what());
                }
            });

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

    CROW_ROUTE(app, "/cross_reference_results.csv")
            .methods("GET"_method)
                    ([](const crow::request& req) {
                        crow::response res;

                        std::ifstream file("cross_reference_results.csv");
                        if (!file) {
                            res.code = 404;
                            res.body = "Results file not found";
                            return res;
                        }

                        std::stringstream buffer;
                        buffer << file.rdbuf();

                        res.set_header("Content-Type", "text/csv");
                        res.set_header("Content-Disposition", "attachment; filename=cross_reference_results.csv");
                        res.body = buffer.str();

                        return res;
                    });

    CROW_ROUTE(app, "/api/feedback")
            .methods("POST"_method)
                    ([&lacerteCrossRef](const crow::request& req) {
                        auto body = crow::json::load(req.body);

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
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") token = token.substr(7);
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    auto metrics = lacerteCrossRef.getModelMetrics();

                    double batchAccuracy = metrics.matchesFound > 0 ?
                                           (double)(metrics.matchesFound + metrics.correctMatches) /
                                           (metrics.matchesFound + metrics.totalHighConfidence) : 0.0;

                    crow::json::wvalue response{
                            {"accuracy", metrics.accuracy},
                            {"batchAccuracy", batchAccuracy},
                            {"totalMatches", metrics.matchesFound},
                            {"learningProgress", metrics.totalPredictions},
                            {"correctMatches", metrics.correctMatches}
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
                    string token = req.get_header_value("Authorization");
                    if (token.substr(0, 7) == "Bearer ") {
                        token = token.substr(7);
                    }
                    if (!auth.validateToken(token)) {
                        res.code = 401;
                        res.body = "Invalid token";
                        return res;
                    }

                    string manager = req.url_params.get("manager") ? req.url_params.get("manager") : "";
                    string group = req.url_params.get("group") ? req.url_params.get("group") : "";
                    string projectType = req.url_params.get("projectType") ? req.url_params.get("projectType") : "";

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