cmake_minimum_required(VERSION 3.10)

project(vct)

option(VCT_FREESTANDING "Enable freestanding mode" OFF)

add_library(vct STATIC src/vct.c)

if(VCT_FREESTANDING)
target_compile_definitions(vct PUBLIC VCT_FREESTANDING)
endif()

target_include_directories(vct PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(tests)