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
#include <cxxtools/jsonserializer.h>
#include <cxxtools/serializationinfo.h>
#include <filesystem>
#include <fstream>
#include <fty_log.h>
#include <sstream>

#define CONFIG_FILE "/etc/fty-alert-srr/fty-alert-srr.cfg"

std::vector<std::pair<std::string, std::string>> getRulesPath()
{
    namespace fs = std::filesystem;

    std::vector<std::pair<std::string, std::string>> pathList = {
        std::make_pair("engine", "/var/lib/fty/fty-alert-engine"),
        std::make_pair("flexible", "/var/lib/fty/fty-alert-flexible/rules"),
        std::make_pair("list", "/var/lib/fty/fty-alert-list")};

    fs::path filePath = CONFIG_FILE;

    if (!fs::exists(filePath)) {
        log_error("Invalid configuration file path: %s", filePath.c_str());
        log_warning("Using default type configuration");

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
        log_warning("Using default type configuration");

        return pathList;
    }

    if (!si.findMember("folders")) {
        log_error("Cannot find configuration for rule types");
        log_warning("Using default type configuration");

        return pathList;
    }

    // clear default values
    pathList.clear();
    cxxtools::SerializationInfo types = si.getMember("folders");

    for (const auto& entry : types) {
        for (const auto& x : entry) {
            try {
                std::string t, p;
                t = x.name();
                x >>= p;
                log_debug("Found new rule type - %s : %s", t.c_str(), p.c_str());
                pathList.push_back(std::make_pair(t, p));
            } catch (std::exception& e) {
                log_error("Error while reading configuration for rule types: %s", e.what());
            }
        }
    }

    return pathList;
}

std::vector<std::pair<std::string, std::string>> getDelExclusions()
{
    namespace fs = std::filesystem;

    std::vector<std::pair<std::string, std::string>> exclusions = {std::make_pair("engine", "warranty")};

    fs::path filePath = CONFIG_FILE;

    if (!fs::exists(filePath)) {
        log_error("Invalid configuration file path: %s", filePath.c_str());
        log_warning("Using default deletion exclusions");

        return exclusions;
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
        log_warning("Using default deletion exclusions");

        return exclusions;
    }

    if (!si.findMember("exclusions")) {
        log_error("Cannot find configuration for rule deletion exclusions");
        log_warning("Using default deletion exclusions");

        return exclusions;
    }

    // clear default values
    exclusions.clear();
    cxxtools::SerializationInfo ex = si.getMember("exclusions");

    for (const auto& entry : ex) {
        for (const auto& x : entry) {
            try {
                std::string t, n;
                t = x.name();
                x >>= n;
                log_debug("Found new deletion exclusion - %s : %s", t.c_str(), n.c_str());
                exclusions.push_back(std::make_pair(t, n));
            } catch (std::exception& e) {
                log_error("Error while reading configuration for rule deletion exclusions: %s", e.what());
            }
        }
    }

    return exclusions;
}
