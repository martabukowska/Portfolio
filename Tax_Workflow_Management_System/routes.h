#pragma once

#include <crow.h>
#include "user_authentification.h"
#include "CSV_management.h"
#include "reminders.h"
#include "Lacerte_cross_ref.h"
#include <vector>

 namespace TaxReturnSystem{

    // Function to set up routes for the web application
    void setupRoutes(crow::SimpleApp& app, Auth& auth, ReminderSystem& reminderSystem, ProjectManager& projectManager, LacerteCrossReference& lacerteCrossRef, ProjectsDatabase& projectsDatabase);

}