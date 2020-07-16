cmake_minimum_required(VERSION 3.13)

##############################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake)

##############################################################################################################

project(fty-alert-srr
VERSION 1.0.0
DESCRIPTION "SRR agent for alert configuration"
)

##############################################################################################################
# set(CMAKE_BUILD_TYPE Debug)

etn_target(exe ${PROJECT_NAME}
    SOURCES
        src/fty-alert-srr.cpp

        src/alert/rule.h
        src/alert/rule.cpp
        src/alert/rule_manager.h
        src/alert/rule_manager.cpp
        src/config/config.h
        src/config/config.cpp
        src/utils/messages.h
        src/utils/messages.cpp
        src/utils/serialization.h
        src/utils/serialization.cpp
        src/srr/alert-srr.h
        src/srr/alert-srr.cpp

    INCLUDE_DIRS
        /usr/include/
        include/
    USES
        cxxtools
        fty_common_dto
        fty_common_logging
        fty_common_messagebus
        fty_proto
        fty_srr
        protobuf
        pthread
        stdc++fs
)

##############################################################################################################
