cmake_minimum_required(VERSION 3.13)

##############################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake)
##############################################################################################################

project(fty-alert-srr
    VERSION 1.0.0
    DESCRIPTION "SRR agent for alert configuration"
)

##############################################################################################################

etn_target(exe ${PROJECT_NAME}
    SOURCES
        src/fty-alert-srr.cpp
    INCLUDE_DIRS
        /usr/include/
        include/
    USES
        fty_common_messagebus
        pthread
    PRIVATE
)

##############################################################################################################
