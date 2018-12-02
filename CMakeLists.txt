cmake_minimum_required(VERSION 3.12)
project(indexer_src)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/lib)
LINK_DIRECTORIES(/usr/local/lib)

find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package (Threads)

include_directories(src/)

add_executable(indexer
        src/indexer.cc
        src/indexer.h
        src/utils.cc
        src/utils.h
        src/ui.cc
        src/ui.h
        src/config.h
        src/config.cc)

target_link_libraries(indexer
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
        )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set_target_properties( indexer
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )