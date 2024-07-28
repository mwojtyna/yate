include(FetchContent)

cmake_minimum_required(VERSION 3.27)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(yate VERSION 0.1.0 LANGUAGES C CXX)
add_compile_options(-Wall -Wextra -Wpedantic -Wfloat-equal -Wundef -Wshadow -Wswitch-enum -Wformat=2)

set(SOURCE_FILES ./src/main.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# glfw
FetchContent_Declare(
		glfw
		GIT_REPOSITORY https://github.com/glfw/glfw
		GIT_TAG        7b6aead # v3.4
)
FetchContent_MakeAvailable(glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_compile_options(glfw PRIVATE -w) # Disable warnings

# opengl
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

# glad
add_library(glad STATIC ./vendor/glad/glad.c)
target_include_directories(glad PUBLIC ./vendor/glad/include/)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
