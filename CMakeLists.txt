cmake_minimum_required (VERSION 3.5)
project (dns-validation)
project (dns-validation-aws)

set (dns-validation_VERSION_MAJOR 1)
set (dns-validation_VERSION_MINOR 0)

add_executable(dns-validation main.cpp dns.h dns.cpp)
add_executable(dns-validation-aws aws.cpp dns.h dns.cpp)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

if(WIN32)
    target_link_libraries(dns-validation dnsapi)
endif()

if(UNIX)
    include(~/vcpkg/scripts/buildsystems/vcpkg.cmake)
    
    set_property(TARGET dns-validation PROPERTY CXX_STANDARD 11)
    set_property(TARGET dns-validation-aws PROPERTY CXX_STANDARD 11)
    
    find_package(CLI11 CONFIG REQUIRED)
    find_package(json-c CONFIG REQUIRED)
    find_package(aws-lambda-runtime REQUIRED)
    
    target_link_libraries(dns-validation resolv CLI11::CLI11)
    target_link_libraries(dns-validation-aws resolv CLI11::CLI11 json-c::json-c AWS::aws-lambda-runtime)

    aws_lambda_package_target(dns-validation-aws)
endif()
