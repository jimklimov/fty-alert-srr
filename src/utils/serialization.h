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

namespace cxxtools {
class SerializationInfo;
}

namespace fty {
class AlertRule;
}

namespace fty { namespace alertutils {

    std::string serializeRules(const std::vector<fty::AlertRule>& rules, const std::string& version);
    std::vector<fty::AlertRule> deserializeRules(const std::string& json, const std::string& version);
}} // namespace fty::alertutils
