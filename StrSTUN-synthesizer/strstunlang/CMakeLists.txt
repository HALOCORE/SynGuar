cmake_minimum_required(VERSION 3.10)
project("strstunlang")

include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(${PROJECT_SOURCE_DIR}/src STRECLANG_SRC)
add_library(strstunlang ${STRECLANG_SRC})
