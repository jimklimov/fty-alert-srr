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

#include "rule.h"
#include <cxxtools/jsondeserializer.h>
#include <cxxtools/jsonserializer.h>
#include <cxxtools/serializationinfo.h>
#include <fty_log.h>
#include <sstream>
#include <string>

static std::string loadData(const std::string& raw)
{
    // std::string json;
    // cxxtools::SerializationInfo si;
    // try {
    //     std::stringstream input;
    //     input << raw;
    //     cxxtools::JsonDeserializer deserializer(input);
    //     deserializer.deserialize(si);
    // } catch (const std::exception& e) {
    //     throw std::runtime_error("Error while deserializing rule data: " + std::string(e.what()));
    // }

    // try {
    //     std::stringstream        output;
    //     cxxtools::JsonSerializer serializer(output);
    //     serializer.serialize(si);

    //     json = output.str();
    // } catch (const std::exception& e) {
    //     throw std::runtime_error("Error while serializing rule data: " + std::string(e.what()));
    // }

    // return json;

    return raw;
}

namespace fty {

AlertRule::AlertRule(const Type& type, const std::string& name, const std::string& data)
    : m_type(type)
    , m_name(name)
    , m_data(loadData(data))
{
}

AlertRule::Type AlertRule::type() const
{
    return m_type;
}

std::string AlertRule::name() const
{
    return m_name;
}

std::string AlertRule::data() const
{
    return m_data;
}

void AlertRule::setType(const Type& type)
{
    m_type = type;
}

void AlertRule::setName(const std::string& name)
{
    m_name = name;
}

void AlertRule::setData(const std::string& data)
{
    m_data = loadData(data);
}

std::string AlertRule::ruleTypeToString(const Type& t)
{
    std::string ret("unknown");

    switch (t) {
        case fty::AlertRule::Type::ENGINE:
            ret = "engine";
            break;
        case fty::AlertRule::Type::FLEXIBLE:
            ret = "flexible";
            break;
        case fty::AlertRule::Type::LIST:
            ret = "list";
            break;
        default:
            break;
    }

    return ret;
}

AlertRule::Type AlertRule::stringToRuleType(const std::string& s)
{
    AlertRule::Type type = AlertRule::Type::UNKNOWN;

    if (s == "engine") {
        type = AlertRule::Type::ENGINE;
    } else if (s == "flexible") {
        type = AlertRule::Type::FLEXIBLE;
    } else if (s == "list") {
        type = AlertRule::Type::LIST;
    }

    return type;
}

void AlertRule::dump(std::ostream& os) const
{
    os << "#################################################################" << std::endl;
    os << "Configuration:" << std::endl;
    os << "\t- Type: " << ruleTypeToString(m_type) << std::endl;
    os << "\t- Name: " << m_name << std::endl;
    os << "\t- Data:" << std::endl;
    os << m_data << std::endl;
    os << "#################################################################" << std::endl;
}

} // namespace fty
