cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(TestProject VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find simdjson package (assuming it's installed or available in a specific location)
find_package(simdjson REQUIRED)

# Find libcurl package (assuming it's installed or available in a specific location)
find_package(CURL REQUIRED)

# Include directories for simdjson and libcurl
include_directories(${SIMDJSON_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})

# Add the executable
add_executable(main src/main.cpp src/ThreadSafeQueue.cpp src/readNSE.cpp)

# Link libraries
target_link_libraries(main PRIVATE simdjson curl)

# Example: Assuming simdjson is installed in /home/schnellecke_test/simdjson
link_directories(/home/schnellecke_test/simdjson/lib)

# Additional paths for reference
set(PATH1 "/home/schnellecke_test/simdjson")
set(PATH2 "/usr/bin/curl")

# Output paths for reference
message("path : ${PATH1}")
message("path : ${PATH2}")
