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

#include "rule_manager.h"
#include "rule.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <fty_log.h>
#include <iostream>
#include <sstream>

namespace fty {

AlertRuleManager::AlertRuleManager()
{
}

void AlertRuleManager::addAlertRulesPath(const std::string& path)
{
    namespace fs = std::filesystem;

    auto found = std::find(m_rulesPath.begin(), m_rulesPath.end(), path);
    if (found != m_rulesPath.end()) {
        log_warning("Path %s is already motitored", path.c_str());
    } else {
        if (fs::exists(path)) {
            m_rulesPath.push_back(path);
            log_debug("Path %s added", path.c_str());
        } else {
            log_error("Path %s does not exist", path.c_str());
        }
    }
}
void AlertRuleManager::removeAlertRulesPath(const std::string& path)
{
    auto found = std::find(m_rulesPath.begin(), m_rulesPath.end(), path);
    if (found != m_rulesPath.end()) {
        log_debug("Path %s deleted", path.c_str());
        m_rulesPath.erase(found);
    } else {
        log_warning("Path %s is not monitored", path.c_str());
    }
}

std::vector<fty::AlertRule> AlertRuleManager::getRules() const
{
    namespace fs = std::filesystem;

    std::vector<fty::AlertRule> rules;

    for (const std::string& path : m_rulesPath) {
        if (!fs::exists(path)) {
            log_error("Path %s does not exist", path.c_str());
            continue;
        }

        for (auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                if (entry.path().extension() == ALERT_RULES_EXT) {
                    log_debug("Found configuration file %s", entry.path().c_str());

                    const std::string& filePath = entry.path().c_str();

                    std::ifstream file;
                    file.open(filePath);

                    std::ostringstream stream;
                    stream << file.rdbuf();

                    rules.push_back(AlertRule(filePath, stream.str()));

                    file.close();
                }
            }
        }
    }
    return rules;
}

void AlertRuleManager::restoreRules(const std::vector<fty::AlertRule>& rules)
{
    namespace fs = std::filesystem;

    for (const auto& r : rules) {
        const std::string& path = r.path();
        const std::string& data = r.data();

        fs::path filePath = path;

        if (fs::exists(path)) {
            log_warning("File %s already exists: skipping...", filePath.filename().c_str());
            continue;
        }

        if (!filePath.has_parent_path()) {
            log_error("Folder %s does not exists: skipping...", filePath.parent_path().c_str());
            continue;
        }

        std::ofstream file;
        file.open(path);

        file << data;

        file.close();
    }
}

} // namespace fty
