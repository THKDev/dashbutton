project(dashbutton)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# add single headers to project structure
FILE(GLOB_RECURSE HeaderFiles "*.hpp")
add_custom_target(headers SOURCES ${HeaderFiles})

include_directories(.)

# use standalone version of ASIO (CentOS7 on arm64 has only a old one)
include_directories(asio/asio/include)
add_definitions(-DASIO_STANDALONE=1)


find_package(Boost 1.53.0 COMPONENTS system program_options REQUIRED)
find_package(Threads)

if(Boost_FOUND)
    aux_source_directory(. SRC_LIST)
    add_executable(${PROJECT_NAME} ${SRC_LIST})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} mosquittopp)
else()
    message(FATAL_ERROR "boost library not found.")
endif()
