cmake_minimum_required(VERSION 3.10)

project(ottakring)

set(CMAKE_CXX_STANDARD 17)

# Import libraries 
find_package(Eigen3 3.3 REQUIRED)


add_executable(demo src/main.cpp)
target_link_libraries(demo Eigen3::Eigen)

