cmake_minimum_required(VERSION 3.0)
project(curve_surfing VERSION 1.0 LANGUAGES C CXX)

#[ OpenGL ]
find_package(OpenGL REQUIRED)

#[ GLFW ]
set(GLFW_DIR external/glfw)
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory(${GLFW_DIR})

#[ glad ]
set(GLAD_DIR
    external/glad
    )
set(GLAD_SOURCES
    external/glad/src/glad.c
    )
add_library(glad ${GLAD_SOURCES})
target_include_directories(glad PRIVATE ${GLAD_DIR}/include)

#[ Headers ]
set(HEADERS
    include/geometry/curve.h
    include/geometry/curvefileio.h
    include/math/vec3f.h
    include/math/mat4f.h
    include/opengl/program.h
    include/opengl/shader.h
    include/opengl/openglmatrix.h
    include/scene/camera.h
    )

#[ Sources ]
set(SOURCES
    src/main.cpp
    src/geometry/curve.cpp
    src/geometry/curvefileio.cpp
    src/math/vec3f.cpp
    src/math/mat4f.cpp
    src/opengl/program.cpp
    src/opengl/shader.cpp
    src/opengl/openglmatrix.cpp
    src/scene/camera.cpp
    )

#[ Resource ]
set(RESOURCE_FILES
    resources/curves/curve.txt
    resources/curves/curve.obj
    resources/curves/coaster.obj
    resources/shaders/basic_vs.glsl
    resources/shaders/basic_fs.glsl
    resources/meshes/cart.obj
    )

configure_file(resources/curves/curve.txt curves/curve.txt COPYONLY)
configure_file(resources/curves/curve.obj curves/curve.obj COPYONLY)
configure_file(resources/curves/coaster.obj curves/coaster.obj COPYONLY)
configure_file(resources/shaders/basic_fs.glsl shaders/basic_fs.glsl COPYONLY)
configure_file(resources/shaders/basic_vs.glsl shaders/basic_vs.glsl COPYONLY)
configure_file(resources/meshes/cart.obj meshes/cart.obj COPYONLY)

#[ Executable ]
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

#[ Definitions ]
target_compile_definitions(${PROJECT_NAME}
    PRIVATE -DGLFW_INCLUDE_NONE
    )

if(MSVC)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE -D_USE_MATH_DEFINES
        )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    MACOSX_BUNDLE TRUE
    MACOSX_FRAMEWORK_IDENTIFIER org.cmake.${PROJECT_NAME}
    )

target_compile_definitions(${PROJECT_NAME}
    PRIVATE GLFW_INCLUDE_NONE
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE ${OPENGL_gl_LIBRARY}
    PRIVATE glfw
    PRIVATE ${GLFW_LIBRARIES}
    PRIVATE glad
    PRIVATE ${GLAD_LIBRARIES}
    PRIVATE ${CMAKE_DL_LIBS}
    )

target_include_directories(${PROJECT_NAME}
    PRIVATE include
    PRIVATE include/geometry
    PRIVATE include/math
    PRIVATE include/opengl
    PRIVATE include/scene
    PRIVATE external
    PRIVATE ${GLFW_DIR}/include
    PRIVATE ${GLAD_DIR}/include
    )
