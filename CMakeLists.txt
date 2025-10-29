# Set the minimum CMake version
cmake_minimum_required(VERSION 3.15)

# Set the project name and C++ standard
project(MyGame VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(raylib_DIR "./build/build_files/")
# Find the Raylib package
add_subdirectory(build/external/raylib-master)

# Add your executable
add_executable(my_game src/main.cpp)
# Link Raylib to your executable
target_link_libraries(my_game PRIVATE raylib)
