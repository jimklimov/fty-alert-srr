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
        src/*.cpp
        src/*.h

    USES
        cxxtools
        fty_common_dto
        fty_common_logging
        fty_common_messagebus
        protobuf
        pthread
        stdc++fs
)

##############################################################################################################

install(FILES conf/fty-alert-srr.cfg DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/fty-alert-srr/)
install(FILES systemd/fty-alert-srr.service DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/systemd/system/)

##############################################################################################################
