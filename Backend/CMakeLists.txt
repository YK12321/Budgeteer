cmake_minimum_required(VERSION 3.10)
project(BudgeteerAPI VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenSSL for HTTPS support
find_package(OpenSSL REQUIRED)

# FetchContent for dependencies
include(FetchContent)

# cpp-httplib for HTTP requests
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.14.0
)

# nlohmann/json for JSON parsing
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(httplib json)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/Item.cpp
    src/Database.cpp
    src/ApiServer.cpp
    src/StoreApiClient.cpp
    src/LLMInterface.cpp
)

# Header files
set(HEADERS
    include/Item.h
    include/Database.h
    include/ApiServer.h
    include/StoreApiClient.h
    include/LLMInterface.h
)

# Create executable
add_executable(BudgeteerAPI ${SOURCES} ${HEADERS})

# Define CPPHTTPLIB_OPENSSL_SUPPORT for SSL support
target_compile_definitions(BudgeteerAPI PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)

# Link libraries
target_link_libraries(BudgeteerAPI PRIVATE 
    httplib::httplib
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Optional: Enable warnings
if(MSVC)
    target_compile_options(BudgeteerAPI PRIVATE /W4)
else()
    target_compile_options(BudgeteerAPI PRIVATE -Wall -Wextra -pedantic)
endif()

# Set output directory
set_target_properties(BudgeteerAPI PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy dataset to build directory
add_custom_command(TARGET BudgeteerAPI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/SampleDataset
    ${CMAKE_BINARY_DIR}/bin/SampleDataset
)
