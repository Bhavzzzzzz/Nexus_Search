# 1. Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# 2. Name your project and specify the language (CXX = C++)
project(DistributedSearchEngine CXX)

# 3. Require C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 4. Add the -O3 optimization flag (just like your g++ command)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# 5. Automatically find the required system libraries
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# 6. Define the final executable name and the .cpp files to compile
# (Note: You do not need to list .h files here)
add_executable(crawler_prototype main.cpp src/URLFrontier.cpp src/BloomFilter.cpp src/TokenBucket.cpp)

# 7. Link the found libraries (curl and pthread) to your executable
target_link_libraries(crawler_prototype PRIVATE CURL::libcurl Threads::Threads)