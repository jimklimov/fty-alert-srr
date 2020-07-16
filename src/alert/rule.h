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

#pragma once

#include <string>
#include <vector>

namespace fty {

class AlertRule
{
public:
    enum class Type
    {
        UNKNOWN,
        ENGINE,
        FLEXIBLE,
        LIST
    };

    AlertRule(const Type& type, const std::string& name, const std::string& data);
    ~AlertRule() = default;

    Type        type() const;
    std::string name() const;
    std::string data() const;

    void setType(const Type& type);
    void setName(const std::string& name);
    void setData(const std::string& data);

    void dump(std::ostream& os) const;

    static std::string ruleTypeToString(const Type& t);
    static Type        stringToRuleType(const std::string& s);

private:
    Type        m_type;
    std::string m_name;
    std::string m_data;
};

} // namespace fty
