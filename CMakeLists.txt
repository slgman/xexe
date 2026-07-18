cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(botbase VERSION 1.0.0)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp

    src/core/engine.cpp
    src/core/tick_driver.cpp
    src/core/task_queue.cpp

    src/macro/macro_engine.cpp

    src/practice/practice_fix.cpp
    src/practice/player_state.cpp

    src/hooks/scheduler_hook.cpp
    src/hooks/director_hook.cpp
    src/hooks/action_manager_hook.cpp
    src/hooks/base_layer_hook.cpp
    src/hooks/play_layer_hook.cpp
    src/hooks/pause_layer_hook.cpp
    src/hooks/player_hook.cpp

    src/util/midhook.cpp
    src/ui/panel.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Set GEODE_SDK to your Geode SDK path")
else()
    message(STATUS "Geode SDK: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

CPMAddPackage("gh:cursey/safetyhook#main")
target_link_libraries(${PROJECT_NAME} safetyhook)

setup_geode_mod(${PROJECT_NAME})
