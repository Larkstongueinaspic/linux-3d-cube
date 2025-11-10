cmake_minimum_required(VERSION 3.15)
project(Rubik3D LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch raylib from GitHub
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
)
FetchContent_Declare(
    min2phase
    GIT_REPOSITORY https://github.com/lilborgo/min2phaseCXX.git
    GIT_TAG master
)

FetchContent_MakeAvailable(min2phase raylib)

# Collect sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(Rubik3D ${SOURCES})

# 包含路径
include_directories(${CMAKE_SOURCE_DIR}/include)

# 链接静态库
target_link_libraries(Rubik3D PRIVATE raylib min2phase)

target_include_directories(Rubik3D PRIVATE
    ${min2phase_SOURCE_DIR}/include
)


# macOS frameworks
if(APPLE)
    find_library(COCOA Cocoa)
    find_library(OpenGL OpenGL)
    target_link_libraries(Rubik3D PRIVATE ${COCOA} ${OpenGL})
endif()
