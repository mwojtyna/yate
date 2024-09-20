function(embed_shader TARGET_NAME SHADER_NAME VARIABLE_NAME)
	set(FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/res/shaders/${SHADER_NAME}")
	get_filename_component(FILE_NAME ${FILE_PATH} NAME)
	set(OUTPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/include/shaders/${FILE_NAME}.hpp")

	add_custom_command(
		OUTPUT ${OUTPUT_FILE}
		COMMAND ${CMAKE_COMMAND}
		-DFILE_PATH=${FILE_PATH}
		-DVARIABLE_NAME=${VARIABLE_NAME}
		-DOUTPUT_FILE=${OUTPUT_FILE}
		-P ${CMAKE_CURRENT_SOURCE_DIR}/EmbedFile.cmake
		DEPENDS ${FILE_PATH}
		COMMENT "Generating embedded file: ${FILE_NAME}"
	)

	target_sources(${TARGET_NAME} PRIVATE ${OUTPUT_FILE})
endfunction()

include(FetchContent)

cmake_minimum_required(VERSION 3.27)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# yate
project(yate VERSION 0.1.0 LANGUAGES C CXX)
file(GLOB_RECURSE SOURCE_FILES ./src/*.cpp)
file(GLOB_RECURSE TEST_DIR ./src/tests/*.cpp)
list(REMOVE_ITEM SOURCE_FILES ${TEST_DIR})
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ./include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	# yate-tests
	file(GLOB_RECURSE TEST_FILES ./src/tests/*.cpp ./src/terminal/*.cpp ./src/utils.cpp)
	set(TESTS_PROJECT_NAME "yate-tests")
	add_executable(${TESTS_PROJECT_NAME} ${TEST_FILES})
endif()

add_compile_options(-Wall -Wextra -Wpedantic -Wfloat-equal -Wundef -Wshadow -Wswitch-enum -Wformat=2)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	add_compile_options(-O2)
endif()

embed_shader(${PROJECT_NAME} "text.vert" "textVertexShader")
embed_shader(${PROJECT_NAME} "text.frag" "textFragmentShader")

# glfw
message(STATUS "Fetching glfw...")
FetchContent_Declare(
		glfw
		GIT_REPOSITORY https://github.com/glfw/glfw
		GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)
target_compile_options(glfw PRIVATE -w) # Disable warnings

# opengl
find_package(OpenGL REQUIRED)
if(APPLE)
	add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

# glad
add_library(glad STATIC ./vendor/glad/glad.c)
target_include_directories(glad PUBLIC ./vendor/glad/include/)
target_compile_options(glad PRIVATE -w) # Disable warnings

# spdlog
message(STATUS "Fetching spdlog...")
FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog
	GIT_TAG v1.14.1
)
FetchContent_MakeAvailable(spdlog)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(spdlog PUBLIC SPDLOG_ACTIVE_LEVEL=0) # trace
else()
	target_compile_definitions(spdlog PUBLIC SPDLOG_ACTIVE_LEVEL=2) # info
endif()
target_compile_options(spdlog PRIVATE -w) # Disable warnings

# glm
message(STATUS "Fetching glm...")
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm
	GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)
target_compile_options(glm PRIVATE -w) # Disable warnings

# imgui v1.91.0
file(GLOB IMGUI_SOURCES ./vendor/imgui/*.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ./vendor/imgui/)
target_link_libraries(imgui PRIVATE glfw)
target_compile_options(imgui PRIVATE -w) # Disable warnings

# freetype
message(STATUS "Fetching freetype...")
FetchContent_Declare(
	freetype
	GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype
	GIT_TAG VER-2-13-2
)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(freetype)
target_compile_definitions(freetype PUBLIC FT_CONFIG_OPTION_ERROR_STRINGS)
target_compile_options(freetype PRIVATE -w) # Disable warnings

# stb
target_include_directories(${PROJECT_NAME} PRIVATE ./vendor/stb/)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	# Setup testing
	message(STATUS "Fetching Catch2...")
	FetchContent_Declare(
		Catch2
		GIT_REPOSITORY https://github.com/catchorg/Catch2
		GIT_TAG        v3.7.1
	)
	FetchContent_MakeAvailable(Catch2)
	target_compile_options(Catch2 PRIVATE -w) # Disable warnings
endif()

# link all libraries
set(LIBS glfw OpenGL::GL glad spdlog glm imgui freetype)
set(INCLUDE_DIRS ./vendor/stb/)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_link_libraries(${TESTS_PROJECT_NAME} PRIVATE spdlog freetype glfw Catch2WithMain)
	target_include_directories(${TESTS_PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
endif()
