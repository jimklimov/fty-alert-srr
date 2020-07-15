/*  =========================================================================
    Copyright (C) 2014 - 2020 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    =========================================================================
*/

#include "config.h"
#include <cxxtools/jsondeserializer.h>
#include <cxxtools/serializationinfo.h>
#include <filesystem>
#include <fstream>
#include <fty_log.h>
#include <sstream>

#define CONFIG_FILE "/etc/fty-alert-srr/fty-alert-srr.cfg"

std::vector<std::string> getRulesPath()
{
    namespace fs = std::filesystem;

    std::vector<std::string> pathList = {
        "/etc/fty-alert-engine", "/etc/fty-alert-flexible", "/etc/fty-alert-stats"};

    fs::path filePath = CONFIG_FILE;

    if (!fs::exists(filePath)) {
        log_error("Invalid configuration file path: %s", filePath.c_str());
        log_warning("Using default configuration");

        return pathList;
    }

    std::ifstream file;
    file.open(filePath);

    std::stringstream json;
    json << file.rdbuf();

    file.close();

    cxxtools::SerializationInfo si;

    try {
        cxxtools::JsonDeserializer deserializer(json);
        deserializer.deserialize(si);
    } catch (const std::exception& e) {
        log_error("Error while deserializing configuration file: %s", e.what());
        log_warning("Using default configuration");

        return pathList;
    }

    si.getMember("folders") >>= pathList;

    return pathList;
}
