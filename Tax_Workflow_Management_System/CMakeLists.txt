# Minimum CMake version and project definition
cmake_minimum_required(VERSION 3.27)
project(THE_PROJECT_2_0)

# Global settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH "/usr/local/cmake")

# PACKAGE FINDING

# Core dependencies
find_package(OpenSSL REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)  # Required by Inja

# Inja setup
set(INJA_INCLUDE_DIR "/opt/homebrew/include")
if(NOT EXISTS "${INJA_INCLUDE_DIR}/inja/inja.hpp")
    message(FATAL_ERROR "Inja header not found at ${INJA_INCLUDE_DIR}/inja/inja.hpp")
endif()

# BCrypt setup
set(BCRYPT_INCLUDE_DIR "/usr/local/include")
set(BCRYPT_LIBRARY "/usr/local/lib/libbcrypt.a")
if(NOT EXISTS "${BCRYPT_INCLUDE_DIR}/bcrypt/BCrypt.hpp")
    message(FATAL_ERROR "BCrypt header not found at ${BCRYPT_INCLUDE_DIR}/bcrypt/BCrypt.hpp")
endif()
if(NOT EXISTS "${BCRYPT_LIBRARY}")
    message(FATAL_ERROR "BCrypt library not found at ${BCRYPT_LIBRARY}")
endif()

# Crow setup
set(CROW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Crow/include")
if(NOT EXISTS "${CROW_INCLUDE_DIR}/crow.h")
    message(FATAL_ERROR "Crow header not found at ${CROW_INCLUDE_DIR}/crow.h")
endif()

# Boost setup (required by Crow)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS system REQUIRED)

# ASIO setup
set(ASIO_INCLUDE_DIR "/opt/homebrew/Cellar/asio/1.30.2/include")
if(NOT EXISTS "${ASIO_INCLUDE_DIR}/asio.hpp")
    message(FATAL_ERROR "ASIO header not found at ${ASIO_INCLUDE_DIR}/asio.hpp")
endif()
message(STATUS "ASIO include dir: ${ASIO_INCLUDE_DIR}")

# Include Directories
include_directories(
        ${OPENSSL_INCLUDE_DIR}
        /usr/local/opt/sqlite/include
        ${CMAKE_SOURCE_DIR}/jwt-cpp/include/picojson
        ${CMAKE_SOURCE_DIR}
        ${BCRYPT_INCLUDE_DIR}
        ${CROW_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${ASIO_INCLUDE_DIR}
        ${INJA_INCLUDE_DIR}
)

# TARGET CONFIGURATION

# Define source files
add_executable(THE_PROJECT_2_0
        main.cpp
        CSV_management.cpp
        CSV_management.h
        report_generator.cpp
        report_generator.h
        statistics.cpp
        statistics.h
        reminders.cpp
        reminders.h
        user_authentification.cpp
        user_authentification.h
        user_database.cpp
        user_database.h
        config.h
        logger.cpp
        logger.h
        email_service.cpp
        email_service.h
        Lacerte_cross_ref.h
        routes.cpp
        routes.h
        user.cpp
        user.h
        config.cpp
)

# Link libraries
target_link_libraries(THE_PROJECT_2_0
        jwt-cpp::jwt-cpp
        OpenSSL::SSL
        OpenSSL::Crypto
        sqlite3
        ${BCRYPT_LIBRARY}
        ${Boost_LIBRARIES}
        nlohmann_json::nlohmann_json
)

# Add include directories to target
target_include_directories(THE_PROJECT_2_0 PRIVATE
        ${BCRYPT_INCLUDE_DIR}
        ${CROW_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${ASIO_INCLUDE_DIR}
        ${INJA_INCLUDE_DIR}
)

# Target properties
set_target_properties(THE_PROJECT_2_0 PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

# Compiler definitions
add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)
target_compile_definitions(THE_PROJECT_2_0 PRIVATE ASIO_STANDALONE)


# POST-BUILD

# Copy templates to build directory
add_custom_command(TARGET THE_PROJECT_2_0 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/templates $<TARGET_FILE_DIR:THE_PROJECT_2_0>/templates
)