cmake_minimum_required(VERSION 3.7)

project(SDL2Test)

cmake_policy(SET CMP0072 NEW)

if (UNIX)
	set(CMAKE_MODULE_PATH ~/.local/lib/cmake)
endif (UNIX)

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


add_executable(SDL2Test "Main.cpp" "Util.cpp" "Util.h" "ShaderProgram.cpp" "ShaderProgram.h" "Texture.h" "Texture.cpp" "Types.h" "Camera.cpp" "Model.h" "Model.cpp" "Material.h" "GridMesh.h" "GridMesh.cpp" "Cylinder.h" "Cylinder.cpp" "SmokeSystem.h" "SmokeSystem.cpp" "Sun.h" "Sun.cpp" "Scene.h" "Scene.cpp" "Flashlight.h" "Flashlight.cpp" "Skybox.h" "Skybox.cpp" "UI.h" "UI.cpp" "Sound.h" "Sound.cpp" "Tree.h" "Tree.cpp" "Tombstone.h" "Tombstone.cpp" "Leaves.h" "Leaves.cpp")
target_link_libraries(SDL2Test ${SDL2_LIBRARIES} ${OPENGL_LIBRARY} ${GLEW_LIBRARIES})
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	add_compile_definitions(DEBUG)
	message(STATUS "Debug build")
else()
	message(STATUS "Non Debug build")
endif()

add_executable(PerlinNoise "PerlinNoise.cpp")
