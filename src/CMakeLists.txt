cmake_minimum_required(VERSION 3.20)

add_executable(SearchEngine main.cpp ${SE_INCLUDE}/converterJSON.h converterJSON.cpp ${SE_INCLUDE}/invertedIndex.h invertedIndex.cpp ${SE_INCLUDE}/searchEngine.h searchEngine.cpp)

target_include_directories(SearchEngine PUBLIC ${SE_INCLUDE})

include(FetchContent)

#----------------------------------------------------------------------------------------------------
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)

FetchContent_MakeAvailable(json)

target_link_libraries(SearchEngine PRIVATE nlohmann_json::nlohmann_json)