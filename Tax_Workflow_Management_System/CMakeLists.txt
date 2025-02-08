cmake_minimum_required(VERSION 3.27)
project(THE_PROJECT_2_0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_PREFIX_PATH "/usr/local/cmake")

# Add dlib first
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/dlib)
option(DLIB_USE_CUDA "Use CUDA" OFF)
option(DLIB_NO_GUI_SUPPORT "No GUI" ON)

# Find required packages
find_package(OpenSSL REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)

# Manually set up Inja
set(INJA_INCLUDE_DIR "/opt/homebrew/include")
if(NOT EXISTS "${INJA_INCLUDE_DIR}/inja/inja.hpp")
    message(FATAL_ERROR "Inja header not found at ${INJA_INCLUDE_DIR}/inja/inja.hpp")
endif()

# Add BCrypt
set(BCRYPT_INCLUDE_DIR "/usr/local/include")
set(BCRYPT_LIBRARY "/usr/local/lib/libbcrypt.a")

if(NOT EXISTS "${BCRYPT_INCLUDE_DIR}/bcrypt/BCrypt.hpp")
    message(FATAL_ERROR "BCrypt header not found at ${BCRYPT_INCLUDE_DIR}/bcrypt/BCrypt.hpp")
endif()

if(NOT EXISTS "${BCRYPT_LIBRARY}")
    message(FATAL_ERROR "BCrypt library not found at ${BCRYPT_LIBRARY}")
endif()

# Add Crow
set(CROW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Crow/include")
if(NOT EXISTS "${CROW_INCLUDE_DIR}/crow.h")
    message(FATAL_ERROR "Crow header not found at ${CROW_INCLUDE_DIR}/crow.h")
endif()

# Crow requires Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS system REQUIRED)

# Add ASIO (header-only)
set(ASIO_INCLUDE_DIR "/opt/homebrew/Cellar/asio/1.30.2/include")
if(NOT EXISTS "${ASIO_INCLUDE_DIR}/asio.hpp")
    message(FATAL_ERROR "ASIO header not found at ${ASIO_INCLUDE_DIR}/asio.hpp")
endif()

# Add executable
add_executable(${PROJECT_NAME}
        main.cpp
        AD_CSV_management.cpp
        AD_CSV_management.h
        AF_report_generator.cpp
        AF_report_generator.h
        AE_statistics.cpp
        AE_statistics.h
        reminders.cpp
        reminders.h
        AA_user_authentification.cpp
        AA_user_authentification.h
        AB_user_database.cpp
        AB_user_database.h
        config.h
        logger.cpp
        logger.h
        email_service.cpp
        email_service.h
        Lacerte_cross_ref.h
        Lacerte_cross_ref.cpp
        routes.cpp
        routes.h
        AC_user.cpp
        AC_user.h
        config.cpp
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
        dlib::dlib
        jwt-cpp::jwt-cpp
        OpenSSL::SSL
        OpenSSL::Crypto
        sqlite3
        ${BCRYPT_LIBRARY}
        ${Boost_LIBRARIES}
        nlohmann_json::nlohmann_json
)

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/third_party/dlib
        ${CMAKE_SOURCE_DIR}/third_party/dlib/dlib/..
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

# Add compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
        BOOST_BIND_GLOBAL_PLACEHOLDERS
        ASIO_STANDALONE
)

# Copy templates folder to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/templates $<TARGET_FILE_DIR:${PROJECT_NAME}>/templates
)