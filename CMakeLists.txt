# Directory Structure
#######
# build/
#   ...
# extern/
#   glad/
#      ...
#   imgui/
#     ...
# src/
#   *.cpp
#   *.h

cmake_minimum_required(VERSION 3.17)

message(${CMAKE_CURRENT_SOURCE_DIR})

set(PROJECT_NAME wasm_gaussian_splatting_renderer)
project(${PROJECT_NAME} VERSION 1.0.0 DESCRIPTION "" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_BINARY_DIR}>)

list(APPEND CMAKE_MODULE_PATH /opt/intel/oneapi/tbb/latest/lib) # Need to link to the TBB library to make "execution" working. Should not be here

# ============================================================================
# PACKAGE INSTALLATION
# ============================================================================

## GLAD
#########
set(GLAD_PATH ${CMAKE_SOURCE_DIR}/extern/glad)
add_library(glad STATIC
        ${GLAD_PATH}/src/glad.c
        ${GLAD_PATH}/include/glad/glad.h
)
target_include_directories(glad
        PUBLIC
        ${GLAD_PATH}/include/
)

## SDL2
##########
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
include_directories(${SDL2_INCLUDE_DIRS})

#find_package(SDL2_image)
#include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# ## IMGUI
# ##########
set(IMGUI_PATH ${CMAKE_SOURCE_DIR}/extern/imgui)

# im-core
add_library(im-core
        ${IMGUI_PATH}/imgui.cpp
        ${IMGUI_PATH}/imgui_demo.cpp
        ${IMGUI_PATH}/imgui_draw.cpp
        ${IMGUI_PATH}/imgui_widgets.cpp
        ${IMGUI_PATH}/imgui_tables.cpp
        ${IMGUI_PATH}/imgui.h
        ${IMGUI_PATH}/imgui_internal.h
)
target_include_directories(im-core
        PUBLIC
        $<BUILD_INTERFACE:${IMGUI_PATH}>
        $<INSTALL_INTERFACE:imgui.h>
        $<INSTALL_INTERFACE:imgui_internal.h>
)

# im-sdl2
add_library(im-sdl2
        ${IMGUI_PATH}/backends/imgui_impl_sdl2.cpp
        ${IMGUI_PATH}/backends/imgui_impl_sdl2.h
)
target_include_directories(im-sdl2
        PUBLIC
        $<BUILD_INTERFACE:${IMGUI_PATH}/backends>
        $<INSTALL_INTERFACE:imgui_impl_sdl2.h>
)
target_link_libraries(im-sdl2
        im-core
        SDL2::SDL2
        glad
)

# im-opengl
add_library(im-opengl3
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.h
)
target_include_directories(im-opengl3
        PUBLIC
        $<BUILD_INTERFACE:${IMGUI_PATH}/backends>
        $<INSTALL_INTERFACE:imgui_impl_opengl3.h>
)
target_link_libraries(im-opengl3
        im-core
        glad
)

# tinyply
set(TINYPLY_PATH ${CMAKE_SOURCE_DIR}/extern/tinyply)
add_library(tinyply
    ${TINYPLY_PATH}/source/tinyply.cpp
    ${TINYPLY_PATH}/source/tinyply.h)

target_include_directories(tinyply
        PUBLIC
        $<BUILD_INTERFACE:${TINYPLY_PATH}/source>)

# glm
add_subdirectory(extern/glm)

# fmt
add_subdirectory(extern/fmt)

# tbb (for execution)
if(NOT EMSCRIPTEN)
find_package(TBB REQUIRED)
endif()

# ============================================================================
# SRC
# ============================================================================
add_executable(${PROJECT_NAME} src/main.cpp
)
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX .html)
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fexperimental-library")
    set_target_properties(${PROJECT_NAME}  PROPERTIES LINK_FLAGS "-std=c++20  -sALLOW_MEMORY_GROWTH -sMIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -sFULL_ES3 --preload-file data/")
endif()

target_sources(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/app.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/load.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/splat.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/camera.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/model.cpp
)

target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        im-core
        im-sdl2
        im-opengl3
        SDL2::SDL2
        glad
        tinyply
        glm::glm
        fmt::fmt
)

if (NOT EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME} PRIVATE TBB::tbb)
endif ()

#set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "$<$<CONFIG:Release>:-O3>")

message(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

ADD_CUSTOM_TARGET(link_target ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR})