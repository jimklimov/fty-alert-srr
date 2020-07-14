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
#include <fty_log.h>
#include <sstream>

namespace fty {

AlertRule::AlertRule(const std::string& path, const std::string& data)
    : m_path(path)
    , m_data(data)
{
}

std::string AlertRule::path() const
{
    return m_path;
}

std::string AlertRule::data() const
{
    return m_data;
}

void AlertRule::dump(std::ostream& os) const
{
    os << "#################################################################" << std::endl;
    os << "Configuration:" << std::endl;
    os << "\t- Path: " << m_path << std::endl;
    os << "\t- Data:" << std::endl;
    os << m_data << std::endl;
    os << "#################################################################" << std::endl;
}

} // namespace fty
