cmake_minimum_required (VERSION 3.5)
project (dns-validation)

set (dns-validation_VERSION_MAJOR 1)
set (dns-validation_VERSION_MINOR 0)

add_executable(dns-validation main.cpp)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

find_package(CLI11 CONFIG REQUIRED)
target_link_libraries(dns-validation CLI11::CLI11)

if(WIN32)
    target_link_libraries(dns-validation dnsapi)
endif()

if(UNIX)
    include(~/vcpkg/scripts/buildsystems/vcpkg.cmake)
    target_link_libraries(dns-validation resolv)
endif()
