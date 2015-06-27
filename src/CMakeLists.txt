cmake_minimum_required(VERSION 2.6)

project(MeshSimplification)

file(GLOB_RECURSE HEADER_FILES "*.h" "*.hpp" "*.inl" "*.pch")
file(GLOB_RECURSE SOURCE_FILES "*.c" "*.cpp")

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
