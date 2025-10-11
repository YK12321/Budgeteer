cmake_minimum_required(VERSION 3.10)
project(BudgeteerAPI VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/Item.cpp
    src/Database.cpp
    src/ApiServer.cpp
)

# Header files
set(HEADERS
    include/Item.h
    include/Database.h
    include/ApiServer.h
)

# Create executable
add_executable(BudgeteerAPI ${SOURCES} ${HEADERS})

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
