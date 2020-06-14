cmake_minimum_required(VERSION 3.15)
project(OpenGLBezier)

set(CMAKE_CXX_STANDARD 11)

add_executable(OpenGLBezier main.cpp Shader.h Object3D.cpp Object3D.h GeometricDefinitions.h GLErrorCheck.h GLErrorCheck.cpp)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(glfw3 3.3 REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
target_include_directories(OpenGLBezier PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
                           $<INSTALL_INTERFACE:include>)
target_link_libraries(OpenGLBezier PUBLIC glfw OpenGL::GL ${CMAKE_DL_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${CMAKE_SOURCE_DIR}/glad/glad.o )