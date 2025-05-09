cmake_minimum_required(VERSION 3.16) 

project(ORbitGFX_PlayGround VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard 20")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(EXTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/external/)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${SRC}/**.cpp")
file(GLOB_RECURSE INCLUDE_FILES CONFIGURE_DEPENDS "${INCLUDE}/**.hpp")

# shaderc
add_subdirectory(${EXTERNAL}/ORbitGFX)

add_executable(GfxText) 

target_compile_definitions(GfxText PUBLIC ASSETS="${CMAKE_CURRENT_SOURCE_DIR}/assets/")

target_sources(GfxText
    PRIVATE
    ${INCLUDE_FILES}
    ${SRC_FILES}
)

target_include_directories(GfxText
    PRIVATE
    "${INCLUDE}"
)

target_link_libraries(GfxText
    PRIVATE
    gfx::ORbit
)

# POST BUILD
copy_renderer_dlls(GfxText)