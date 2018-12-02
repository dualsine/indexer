cmake_minimum_required(VERSION 3.12)
project(indexer_src)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/lib)
LINK_DIRECTORIES(/usr/local/lib)

find_package(Boost COMPONENTS system filesystem REQUIRED)

set(PostgreSQL_ADDITIONAL_VERSIONS "11")
set(PostgreSQL_INCLUDE_DIR /usr/include/postgresql)
set(PostgreSQL_LIBRARY_DIR /usr/include/postgresql)
find_package(PostgreSQL REQUIRED)

include_directories(.)

add_executable(indexer
        indexer.cc
        indexer.h)

target_link_libraries(indexer
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${PostgreSQL_LIBRARIES}
        marisa
        )

target_include_directories(indexer PRIVATE ${PostgreSQL_INCLUDE_DIRS})