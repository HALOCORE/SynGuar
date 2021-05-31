cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 11)

project(strstun)
# find_package(gmp PATHS "/usr/lib/x86_64-linux-gnu/")
# include_directories(${GOBJECT_INCLUDE_DIR})

# add_definitions(-D_GLIBCXX_DEBUG)
# SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-limit-debug-info")
# SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-undefined-inline")

set(GMP_LIB ${CMAKE_CURRENT_LIST_DIR}/dependencies/libs/_gmp621/lib/libgmp.a)
set(GMPXX_LIB ${CMAKE_CURRENT_LIST_DIR}/dependencies/libs/_gmp621/lib/libgmpxx.a)
include_directories(${CMAKE_CURRENT_LIST_DIR}/dependencies/libs/_gmp621/include)

add_subdirectory(strstunlib0)
include_directories(${PROJECT_SOURCE_DIR}/strstunlib0/include) 

add_subdirectory(strstunlang)
include_directories(${PROJECT_SOURCE_DIR}/strstunlang/include) 

add_executable(strstun main.cpp)
target_link_libraries(strstun strstunlib0 strstunlang ${GMPXX_LIB} ${GMP_LIB}) #${GOBJECT_LIBRARIES}

add_executable(oracle oracle.cpp)
target_link_libraries(oracle strstunlang)

