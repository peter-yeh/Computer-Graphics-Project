cmake_minimum_required(VERSION 3.7...3.18)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Set up the project
project(Lab2 VERSION 1.0
             DESCRIPTION "CS3241 Lab Assignment 2"
             LANGUAGES CXX)

# Suppress generation of ZERO_CHECK build target
set(CMAKE_SUPPRESS_REGENERATION true)

# Add the executable
add_executable(${PROJECT_NAME} main.cpp)

# Set the output directory to the top-level directory of the project
# without any Debug, Release, etc folders, so the freeglut.dll file can be read by the exe.
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/$<0:>)


if(WIN32 AND MSVC)
    # Use provided libraries and headers if Windows and MSVC
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include/)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/freeglut.lib)
elseif(APPLE OR (CMAKE_SYSTEM_NAME MATCHES "Linux"))
    # When on MacOS/Linux, assume students have installed FreeGLUT with a package manager, or
    # GLUT is already installed with the OS, and link the appropriate directories and libraries.
    find_package(GLUT REQUIRED)
    find_package(OpenGL REQUIRED)

    if(${GLUT_FOUND} AND ${OPENGL_FOUND})
        target_include_directories(${PROJECT_NAME} PRIVATE ${GLUT_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})
    else()
        message(FATAL_ERROR "(Free)GLUT or OpenGL not found; generation halted.")
    endif()
endif()
