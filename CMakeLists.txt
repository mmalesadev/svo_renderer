cmake_minimum_required(VERSION 3.6)

### Project information
project(svo-renderer)
set(TheLastCityOnEarth_VERSION_MAJOR 1)
set(TheLastCityOnEarth_VERSION_MINOR 0)
set(TARGET_NAME svo-renderer)

### Source information
set(ROOT_SRC_DIR "src")

# This function adds directory to the variable set below.
# Unset if you want to use it again
set(ALL_SRCS)
function(add_directory directory)
	include_directories(${CMAKE_SOURCE_DIR}/${directory})
	file(GLOB DIR_SRCS ${CMAKE_SOURCE_DIR}/${directory}/*.cpp ${CMAKE_SOURCE_DIR}/${directory}/*.h)
	string(REPLACE "/" "\\" directory_out ${directory})
	source_group(${directory_out} FILES ${DIR_SRCS})
	set(ALL_SRCS ${ALL_SRCS} ${DIR_SRCS} PARENT_SCOPE)
endfunction(add_directory)

add_directory(${ROOT_SRC_DIR})
add_directory("${ROOT_SRC_DIR}/systems")
add_directory("${ROOT_SRC_DIR}/scenes")
add_directory("${ROOT_SRC_DIR}/entities")
add_directory("${ROOT_SRC_DIR}/components")
add_directory("${ROOT_SRC_DIR}/world")
add_directory("${ROOT_SRC_DIR}/context")

### Libraries and headers
set(LIB_DIR "lib")

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	if( WIN32 )
		set( TARGET_SYSTEM "win64" )
	elseif( UNIX )
		set( TARGET_SYSTEM "linux64" )
	endif()
endif()

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})


# GLEW
set(DEFAULT_GLEW_DIR "glew-2.0.0")
find_path(GLEW_HEADERS_DIR GL/glew.h ${LIB_DIR}/${DEFAULT_GLEW_DIR}/include)
find_library(GLEW_LIB NAMES glew32 libGLEW.a PATHS ${LIB_DIR}/${DEFAULT_GLEW_DIR}/${TARGET_SYSTEM})
if(GLEW_HEADERS_DIR AND GLEW_LIB)
	set(LIBS ${LIBS} ${GLEW_LIB})
	include_directories( ${GLEW_HEADERS_DIR} )
else()
	message(SEND_ERROR "Glew was not found.")
endif()

# GLFW
set(DEFAULT_GLFW_DIR "glfw-3.2.1")
find_path(GLFW_HEADERS_DIR GLFW/glfw3.h ${LIB_DIR}/${DEFAULT_GLFW_DIR}/include)
find_library(GLFW_LIB NAMES glfw3 libglfw3.a PATHS ${LIB_DIR}/${DEFAULT_GLFW_DIR}/${TARGET_SYSTEM})
if(GLFW_HEADERS_DIR AND GLFW_LIB)
	set(LIBS ${LIBS} ${GLFW_LIB})
	include_directories( ${GLFW_HEADERS_DIR} )
else()
	message(SEND_ERROR "GLFW was not found.")
endif()

# GLM
set(DEFAULT_GLM_DIR "glm")
find_path(GLM_HEADERS_DIR glm/glm.hpp ${LIB_DIR}/${DEFAULT_GLM_DIR})
if(GLM_HEADERS_DIR)
	include_directories( ${GLM_HEADERS_DIR} )
else()
	message(SEND_ERROR "GLM was not found.")
endif()

# Catch
set(DEFAULT_CATCH_DIR "catch-1.9.1")
find_path(CATCH_HEADERS_DIR catch.hpp ${LIB_DIR}/${DEFAULT_CATCH_DIR})
if(CATCH_HEADERS_DIR)
	include_directories( ${CATCH_HEADERS_DIR} )
else()
	message(SEND_ERROR "Catch was not found.")
endif()


### Link target
add_executable(${TARGET_NAME} ${ALL_SRCS})
set(LINKER_FLAGS)
if(UNIX)
	set(LINKER_FLAGS ${LINKER_FLAGS} "-ldl -lpthread -lX11 -lXrandr -lXinerama -lXxf86vm -lXcursor")
endif()
target_link_libraries(${TARGET_NAME} ${LIBS} ${LINKER_FLAGS})

if(MSVC)
	### Setting target as startup project
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

	### Copying DLLs target
	file(GLOB DLLS ${CMAKE_SOURCE_DIR}/dll/${TARGET_SYSTEM}/*.dll)
	add_custom_target(CopyDlls ALL DEPENDS ${CMAKE_BINARY_DIR}/unexisting_file.h)
	foreach(DLL ${DLLS})	
	  add_custom_command(TARGET CopyDlls POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${DLL} ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/)
	endforeach()
endif()

### TEST PROJECT
project(svo-renderer-tests)
set(TESTS_TARGET_NAME svo-renderer-tests)

# Tests dir
set(ROOT_TESTS_DIR "test")

unset(ALL_SRCS)
add_directory(${ROOT_TESTS_DIR})
add_directory("${ROOT_SRC_DIR}/systems")
add_directory("${ROOT_SRC_DIR}/scenes")
add_directory("${ROOT_SRC_DIR}/entities")
add_directory("${ROOT_SRC_DIR}/components")
add_directory("${ROOT_SRC_DIR}/world")
add_directory("${ROOT_SRC_DIR}/context")

add_executable(${TESTS_TARGET_NAME} ${ALL_SRCS})
target_link_libraries(${TESTS_TARGET_NAME} ${LIBS} ${LINKER_FLAGS})
