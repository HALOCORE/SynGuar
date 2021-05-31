cmake_minimum_required(VERSION 3.10)
project("strstunlib0")

include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(${PROJECT_SOURCE_DIR}/src STRECLIB0_SRC)

include_directories(${PROJECT_SOURCE_DIR}/../strstunlang/include)

add_library(strstunlib0 ${STRECLIB0_SRC})
