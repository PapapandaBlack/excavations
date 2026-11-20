cmake_minimum_required(VERSION 3.28)

project(Excavations LANGUAGES CXX)

include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "Build SFML audio" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "Build SFML network" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SFML)

add_executable(Excavations
    src/main.cpp
    src/core/Application.cpp
    src/game/Player.cpp
    src/game/DesertExcavation.cpp
    src/game/UnderwaterExcavation.cpp
    src/game/PolarExcavation.cpp
    src/game/SpaceExcavation.cpp
)

target_compile_features(Excavations PRIVATE cxx_std_17)
set_target_properties(Excavations PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(Excavations PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(Excavations PRIVATE SFML::Graphics)

if(MSVC)
    target_compile_options(Excavations PRIVATE /utf-8)
endif()

file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")

add_custom_target(ExcavationsAssets
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:Excavations>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Excavations>/assets"
    COMMENT "Copying game assets"
    VERBATIM
)

add_dependencies(Excavations ExcavationsAssets)

set_target_properties(Excavations PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Excavations>"
)
