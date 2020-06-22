cmake_minimum_required (VERSION 3.5.1) # Not sure what the minimum is required
project (givr_example CXX C)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Use modern C++
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

include_directories("${PROJECT_BINARY_DIR}" libs src)

set(DEFINITIONS _USE_MATH_DEFINES=1 GLM_FORCE_CXX14=1)

if(UNIX)
    # setup warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type")

    # add extra checks for developer or debug builds
    if(CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_ASSERTIONS")
    endif()
endif()

find_package(OpenGL REQUIRED)
set(LIBRARIES ${LIBRARIES} ${OPENGL_gl_LIBRARY})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/glfw)
set(LIBRARIES ${LIBRARIES} glfw)

file(GLOB sources src/*.cpp src/*.h libs/*.h libs/*.cpp libs/*.c)

file(GLOB_RECURSE models RELATIVE ${CMAKE_SOURCE_DIR} models/*)
foreach(file ${models})
    configure_file(${file} ${file} COPYONLY)
endforeach(file)

add_executable(simple ${sources} ${example_source})
target_link_libraries(simple ${LIBRARIES})
target_include_directories(simple PRIVATE ${INCLUDES})
target_compile_definitions(simple PRIVATE ${DEFINITIONS})
