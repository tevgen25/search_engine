cmake_minimum_required(VERSION 4.00)
project(search_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)

FetchContent_MakeAvailable(json googletest)

add_executable(search_engine
        main.cpp
        src/ConverterJSON.cpp
        src/InvertedIndex.cpp
        src/SearchServer.cpp
)

target_include_directories(search_engine PRIVATE include)
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)

enable_testing()
add_executable(search_engine_tests
        test/test_main.cpp
        src/ConverterJSON.cpp
        src/InvertedIndex.cpp
        src/SearchServer.cpp
)

target_include_directories(search_engine_tests PRIVATE include)
target_link_libraries(search_engine_tests PRIVATE
        gtest_main
        nlohmann_json::nlohmann_json
)

include(GoogleTest)
gtest_discover_tests(search_engine_tests)