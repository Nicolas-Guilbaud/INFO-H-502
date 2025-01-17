project(INFO-H-502-Project)

# Configure CMake & C++ version
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)     
set(CMAKE_VERBOSE_MAKEFILE ON)

# openGL lib
find_package(OpenGL REQUIRED)

#for glad library
add_library(glad STATIC 3rdParty/glad/src/glad.c)
set(GLAD_INCLUDE "3rdParty/glad/include")
target_include_directories(glad PUBLIC ${GLAD_INCLUDE})

#for GLFW
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

#Search for glfw in case it is installed
find_package(glfw3 QUIET)
if (NOT glfw3_FOUND)
    #if glfw is not found, we will use the sources from the submodules
    add_subdirectory(3rdParty/glfw)
    include_directories(3rdParty/glfw/include/)
endif()


# include libs to project
include_directories(3rdParty/glad/include/
                    3rdParty/glm/
                    3rdParty/stb/)


# Defining usefull Macros for the project
add_compile_definitions(PATH_TO_MESHES="${CMAKE_CURRENT_SOURCE_DIR}/resources/meshes")
add_compile_definitions(PATH_TO_TEXTURES="${CMAKE_CURRENT_SOURCE_DIR}/resources/textures")
add_compile_definitions(PATH_TO_SHADERS="${CMAKE_CURRENT_SOURCE_DIR}/src/shaders")

# Compilation

# recursive search for cpp & header files
file(GLOB_RECURSE CPP_FILES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")
file(GLOB_RECURSE H_FILES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.h")

# Debug msg to see what files are linked in the executable
message(
    "Building project using:\n"
    "- headers: ${H_FILES}\n"
    "- code: ${CPP_FILES}"
)

add_executable(${PROJECT_NAME}_runnable ${CPP_FILES} ${H_FILES})
target_link_libraries(${PROJECT_NAME}_runnable PUBLIC OpenGL::GL glfw glad)
