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

#include "rule.h"
#include <map>
#include <string>
#include <vector>

namespace fty {

static constexpr const char* ALERT_RULES_EXT = ".rule";

class AlertRuleManager
{
public:
    using TypeNamePair = std::pair<AlertRule::Type, std::string>;

    AlertRuleManager();
    ~AlertRuleManager() = default;

    void storeRule(const AlertRule& r, bool overwrite = false);
    void deleteRule(const AlertRule& r);

    void addAlertRulesPath(const fty::AlertRule::Type& type, const std::string& path);
    void removeAlertRulesPath(const fty::AlertRule::Type& type);
    
    void addDelExclusion(const fty::AlertRule::Type& type, const std::string& name);
    void removeDelExclusion(const fty::AlertRule::Type& type, const std::string& name);

    std::vector<fty::AlertRule> getRules() const;
    void                        restoreRules(const std::vector<fty::AlertRule>& rules);
    void                        clearRules();

private:
    std::map<fty::AlertRule::Type, std::string> m_typePath;
    std::vector<TypeNamePair>                   m_delExclusions;
};

} // namespace fty
