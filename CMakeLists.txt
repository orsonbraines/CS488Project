cmake_minimum_required(VERSION 3.7)

project(SDL2Test)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
message(STATUS "SDL2_INCLUDE_DIRS=${SDL2_INCLUDE_DIRS}")

find_package(OpenGL REQUIRED)

find_package(GLM REQUIRED)
message(STATUS "GLM_INCLUDE_DIR=${GLM_INCLUDE_DIR}")
include_directories(${GLM_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
message(STATUS "GLEW_INCLUDE_DIRS=${GLEW_INCLUDE_DIRS}")


add_executable(SDL2Test "Main.cpp" "Util.cpp" "Util.h" "ShaderProgram.cpp" "ShaderProgram.h" "Texture.h" "Texture.cpp" "Types.h")
target_link_libraries(SDL2Test ${SDL2_LIBRARIES} ${OPENGL_LIBRARY} ${GLEW_LIBRARIES})
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	add_compile_definitions(DEBUG)
	message(STATUS "Debug build")
else()
	message(STATUS "Non Debug build")
endif()