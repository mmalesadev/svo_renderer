cmake_minimum_required(VERSION 3.9)

# This function adds directory to the variable set below.
# Unset if you want to use it again
set(ALL_SRCS)
function(add_directory directory)
    include_directories(${CMAKE_SOURCE_DIR}/${directory})
    file(GLOB DIR_SRCS ${CMAKE_SOURCE_DIR}/${directory}/*.cpp ${CMAKE_SOURCE_DIR}/${directory}/*.h ${CMAKE_SOURCE_DIR}/${directory}/*.hpp)
    string(REPLACE "/" "\\" directory_out ${directory})
    source_group(${directory_out} FILES ${DIR_SRCS})
    set(ALL_SRCS ${ALL_SRCS} ${DIR_SRCS} PARENT_SCOPE)
endfunction(add_directory)

### Project information
project(svo_renderer)
set(CMAKE_CXX_STANDARD 17)

### Source information
set(ROOT_SRC_DIR "src")

add_directory(${ROOT_SRC_DIR})
add_directory("${ROOT_SRC_DIR}/systems")
add_directory("${ROOT_SRC_DIR}/systems/rendering")
add_directory("${ROOT_SRC_DIR}/core")
add_directory("${ROOT_SRC_DIR}/entities")
add_directory("${ROOT_SRC_DIR}/components")
add_directory("${ROOT_SRC_DIR}/utility")
add_directory("${ROOT_SRC_DIR}/gui")
add_directory("${ROOT_SRC_DIR}/core")

# Shaders
file(GLOB SHADER_FILES ${CMAKE_SOURCE_DIR}/data/shaders/*.vert ${CMAKE_SOURCE_DIR}/data/shaders/*.frag ${CMAKE_SOURCE_DIR}/data/shaders/*.geom)
source_group("shaders" FILES ${SHADER_FILES})

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

# Spdlog
set(DEFAULT_SPDLOG_DIR "spdlog-0.13.0")
find_path(SPDLOG_HEADERS_DIR spdlog/logger.h ${LIB_DIR}/${DEFAULT_SPDLOG_DIR})
if(SPDLOG_HEADERS_DIR)
    include_directories( ${SPDLOG_HEADERS_DIR} )
else()
    message(SEND_ERROR "Spdlog was not found.")
endif()

# Libmorton
set(DEFAULT_LIBMORTON_DIR "libmorton")
find_path(LIBMORTON_HEADERS_DIR morton.h ${LIB_DIR}/${DEFAULT_LIBMORTON_DIR}/include)
if(LIBMORTON_HEADERS_DIR)
    include_directories( ${LIBMORTON_HEADERS_DIR} )
else()
    message(SEND_ERROR "Libmorton was not found.")
endif()

# Xtl
set(DEFAULT_XTL_DIR "xtl")
find_path(XTL_HEADERS_DIR xtl/xsequence.hpp ${LIB_DIR}/${DEFAULT_XTL_DIR}/include)
if(XTL_HEADERS_DIR)
    include_directories( ${XTL_HEADERS_DIR} )
else()
    message(SEND_ERROR "Xtl was not found.")
endif()

# xtensor
set(DEFAULT_XTENSOR_DIR "xtensor")
find_path(XTENSOR_HEADERS_DIR xtensor.hpp ${LIB_DIR}/${DEFAULT_XTENSOR_DIR}/include)
if(XTENSOR_HEADERS_DIR)
    include_directories( ${XTENSOR_HEADERS_DIR} )
else()
    message(SEND_ERROR "Xtensor was not found.")
endif()

# FreeImage
set(DEFAULT_FREEIMAGE_DIR "FreeImage")
find_path(FREEIMAGE_HEADERS_DIR FreeImage.h ${LIB_DIR}/${DEFAULT_FREEIMAGE_DIR}/include)
find_library(FREEIMAGE_LIB FreeImage ${LIB_DIR}/${DEFAULT_FREEIMAGE_DIR}/${TARGET_SYSTEM})
if(FREEIMAGE_HEADERS_DIR AND FREEIMAGE_LIB)
    set(LIBS ${LIBS} ${FREEIMAGE_LIB})
    include_directories( ${FREEIMAGE_HEADERS_DIR} )
else()
    message(SEND_ERROR "FreeImage was not found.")
endif()

# STB
set(DEFAULT_STB_DIR "stb")
find_path(STB_HEADERS_DIR stb_image.h ${LIB_DIR}/${DEFAULT_STB_DIR})
if(STB_HEADERS_DIR)
    include_directories( ${STB_HEADERS_DIR} )
else()
    message(SEND_ERROR "STB was not found.")
endif()

# Json
set(DEFAULT_JSON_DIR "json")
find_path(JSON_HEADERS_DIR json.h ${LIB_DIR}/${DEFAULT_JSON_DIR}/include)
if(JSON_HEADERS_DIR)
    include_directories( ${JSON_HEADERS_DIR} )
else()
    message(SEND_ERROR "JSON was not found.")
endif()

### Link target
#if(MSVC)
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif()
add_executable(svo_renderer "")
target_sources(svo_renderer PRIVATE ${ALL_SRCS} ${SHADER_FILES})

set(LINKER_FLAGS)
if(UNIX)
    set(LINKER_FLAGS ${LINKER_FLAGS} "-ldl -lpthread -lX11 -lXrandr -lXinerama -lXxf86vm -lXcursor")
endif()
target_link_libraries(svo_renderer ${LIBS} ${LINKER_FLAGS})

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
add_directory("${ROOT_SRC_DIR}/utility")
add_directory("${ROOT_SRC_DIR}/gui")

add_executable(svo_renderer_tests ${ALL_SRCS} ${SHADER_FILES})
target_link_libraries(svo_renderer_tests ${LIBS} ${LINKER_FLAGS})