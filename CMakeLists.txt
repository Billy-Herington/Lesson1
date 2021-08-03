cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(Lesson1)

add_executable(Lesson1
	src/main.cpp
	src/shader.h
	src/camera.h
	src/stb_image.h
	src/stb_image.cpp
	src/mesh.h
	src/model.h
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
target_link_libraries(Lesson1 glfw)

add_subdirectory(external/glad)
target_link_libraries(Lesson1 glad)

add_subdirectory(external/assimp)
target_link_libraries(Lesson1 assimp)

include_directories(external/glm)