cmake_minimum_required(VERSION 3.22)

project(Amethisty VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(amethisty SHARED
    src/main.cpp
    src/android/game_runtime.cpp
    src/core/mod_manager.cpp
    src/core/manifest.cpp
)

target_include_directories(amethisty PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/jsmn
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_options(amethisty PRIVATE
    -Wall
    -Wextra
    -fvisibility=hidden
)

if(ANDROID)
    target_link_libraries(amethisty PRIVATE log dl)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(amethisty_manifest_tests
        tests/manifest_test.cpp
        src/core/manifest.cpp
    )
    target_include_directories(amethisty_manifest_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/jsmn
    )
    target_compile_options(amethisty_manifest_tests PRIVATE -Wall -Wextra)
    add_test(NAME amethisty_manifest_tests COMMAND amethisty_manifest_tests)
endif()
