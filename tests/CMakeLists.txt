cmake_minimum_required(VERSION 3.20)

add_executable(TestEngine Test1.cpp ${SE_INCLUDE}/converterJSON.h ${SE_SRC}/converterJSON.cpp ${SE_INCLUDE}/invertedIndex.h ${SE_SRC}/invertedIndex.cpp ${SE_INCLUDE}/searchEngine.h ${SE_SRC}/searchEngine.cpp)

target_include_directories(TestEngine PUBLIC ${SE_INCLUDE})
#---------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(gtest_disable_pthreads on)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

target_link_libraries(TestEngine PRIVATE gtest_main)

include(GoogleTest)
#---------------------------------------------------------


gtest_discover_tests(TestEngine)
#----------------------------------------------------------------------------------------------------
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)

FetchContent_MakeAvailable(json)

target_link_libraries(TestEngine PRIVATE nlohmann_json::nlohmann_json)

