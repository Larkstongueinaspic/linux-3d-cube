cmake_minimum_required(VERSION 3.15)
project(Rubik3D LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
)
FetchContent_MakeAvailable(raylib)

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(Rubik3D ${SOURCES})

# link raylib; if using pkg-config on system, change as needed
target_link_libraries(Rubik3D PRIVATE raylib)
if(APPLE)
    # macOS: link frameworks if needed
    find_library(COCOA Cocoa)
    find_library(OpenGL OpenGL)
    target_link_libraries(Rubik3D PRIVATE ${COCOA} ${OpenGL})
endif()