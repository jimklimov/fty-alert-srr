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

#include "serialization.h"
#include "src/alert/rule.h"
#include <cxxtools/jsondeserializer.h>
#include <cxxtools/jsonserializer.h>
#include <cxxtools/serializationinfo.h>
#include <iostream>
#include <sstream>
#include <string>

namespace fty { namespace alertutils {
    std::string serializeRules(const std::vector<fty::AlertRule>& rules, const std::string& version)
    {
        cxxtools::SerializationInfo si;
        si.addMember("version") <<= version;

        cxxtools::SerializationInfo& data = si.addMember("data");

        for (auto& r : rules) {
            cxxtools::SerializationInfo& ruleSi = data.addMember("");
            ruleSi.addMember("type") <<= AlertRule::ruleTypeToString(r.type());
            ruleSi.addMember("name") <<= r.name();
            ruleSi.addMember("data") <<= r.data();
        }

        data.setCategory(cxxtools::SerializationInfo::Array);

        std::string json;

        try {
            std::stringstream        output;
            cxxtools::JsonSerializer serializer(output);
            serializer.serialize(si);

            json = output.str();
        } catch (const std::exception& e) {
            throw std::runtime_error("Error while serializing alert rules " + std::string(e.what()));
        }

        return json;
    }

    std::vector<fty::AlertRule> deserializeRules(const std::string& json, const std::string& version)
    {
        std::vector<AlertRule> rules;

        cxxtools::SerializationInfo si;

        try {
            std::stringstream input;
            input << json;
            cxxtools::JsonDeserializer deserializer(input);
            deserializer.deserialize(si);
        } catch (const std::exception& e) {
            throw std::runtime_error("Error while deserializing alert rules: " + std::string(e.what()));
        }

        std::string srrVersion;
        si.getMember("version") >>= srrVersion;

        if (srrVersion != version) {
            throw std::runtime_error("SRR version " + srrVersion + " is not supported");
        }

        const cxxtools::SerializationInfo& data = si.getMember("data");

        for (auto it = data.begin(); it != data.end(); ++it) {
            std::string t, n, d;

            it->getMember("type") >>= t;
            it->getMember("name") >>= n;
            it->getMember("data") >>= d;
            AlertRule r(AlertRule::stringToRuleType(t), n, d);

            rules.push_back(r);
        }

        return rules;
    }
}} // namespace fty::alertutils
