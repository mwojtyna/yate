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

project(yate VERSION 0.1.0 LANGUAGES C CXX)
add_compile_options(-Wall -Wextra -Wpedantic -Wfloat-equal -Wundef -Wshadow -Wswitch-enum -Wformat=2)

set(
	SOURCE_FILES
	./src/main.cpp
	./src/application.cpp
	./src/renderer.cpp
	./src/program.cpp
	./src/error.cpp
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_compile_options(-Wall -Wextra -Wpedantic -Wfloat-equal -Wundef -Wshadow -Wswitch-enum -Wformat=2)
target_include_directories(${PROJECT_NAME} PUBLIC ./include)

embed_shader(${PROJECT_NAME} "vertex.vert" vertex_shader)
embed_shader(${PROJECT_NAME} "fragment.frag" fragment_shader)

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
if(APPLE)
	add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

# glad
add_library(glad STATIC ./vendor/glad/glad.c)
target_include_directories(glad PUBLIC ./vendor/glad/include/)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_compile_options(glad PRIVATE -w) # Disable warnings

# spdlog
FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog
	GIT_TAG 27cb4c7 # v1.14.1
)
FetchContent_MakeAvailable(spdlog)
target_compile_definitions(spdlog PUBLIC SPDLOG_ACTIVE_LEVEL=0) # trace
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
target_compile_options(spdlog PRIVATE -w) # Disable warnings


