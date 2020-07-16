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

void AlertRuleManager::storeRule(const AlertRule& r, bool overwrite)
{
    namespace fs = std::filesystem;

    const fty::AlertRule::Type type = r.type();
    const std::string&         name = r.name();
    const std::string&         data = r.data();

    const std::string typeStr = AlertRule::ruleTypeToString(type);

    if (!m_typePath.count(type)) {
        throw std::runtime_error("Type " + AlertRule::ruleTypeToString(type) +
                                 " is not monitored: cannot create rule " + typeStr + ":" + name);
    }

    fs::path filePath = m_typePath[type];
    filePath.append(name + ALERT_RULES_EXT);

    if (fs::exists(filePath)) {
        if (!overwrite) {
            throw std::runtime_error("File " + filePath.filename().string() +
                                     " already exists. Skipping creation of rule " + typeStr + ":" + name);
        } else {
            log_warning("File %s already exists : overwrite", filePath.filename().c_str());
        }
    }

    if (!fs::exists(filePath.parent_path())) {
        throw std::runtime_error("Folder " + filePath.parent_path().string() + " does not exist");
    }

    log_debug("Creating rule %s:%s", AlertRule::ruleTypeToString(type).c_str(), name.c_str());

    std::ofstream file;
    file.open(filePath);

    file << data;

    file.close();
}

void AlertRuleManager::deleteRule(const AlertRule& r)
{
    namespace fs = std::filesystem;

    fs::path filePath = m_typePath[r.type()];

    filePath.append(r.name() + ALERT_RULES_EXT);

    fs::remove(filePath);
}

void AlertRuleManager::addAlertRulesPath(const fty::AlertRule::Type& type, const std::string& path)
{
    namespace fs = std::filesystem;

    auto found = m_typePath.find(type);
    if (found != m_typePath.end()) {
        log_warning("Type %s already exists", AlertRule::ruleTypeToString(type).c_str());
    } else {
        if (fs::exists(path)) {
            m_typePath[type] = path;
        } else {
            log_error("Path %s does not exist", path.c_str());
        }
    }
}

void AlertRuleManager::removeAlertRulesPath(const fty::AlertRule::Type& type)
{
    auto found = m_typePath.find(type);

    if (found != m_typePath.end()) {
        m_typePath.erase(found);
        log_debug("Type %s deleted", AlertRule::ruleTypeToString(type).c_str());
    } else {
        log_warning("Type %s is not monitored", AlertRule::ruleTypeToString(type).c_str());
    }
}

void AlertRuleManager::addDelExclusion(const fty::AlertRule::Type& type, const std::string& name)
{
    auto found = std::find(m_delExclusions.begin(), m_delExclusions.end(), std::make_pair(type, name));

    if (found != m_delExclusions.end()) {
        log_warning("Rule %s:%s already excluded from deletion", AlertRule::ruleTypeToString(type).c_str(),
            name.c_str());
    } else {
        m_delExclusions.push_back(std::make_pair(type, name));
    }
}

void AlertRuleManager::removeDelExclusion(const fty::AlertRule::Type& type, const std::string& name)
{
    auto found = std::find(m_delExclusions.begin(), m_delExclusions.end(), std::make_pair(type, name));

    if (found != m_delExclusions.end()) {
        log_debug(
            "Deleting rule %s:%s from exclusions", AlertRule::ruleTypeToString(type).c_str(), name.c_str());
        m_delExclusions.erase(found);
    } else {
        log_warning(
            "Rule %s:%s is not set in exclusions", AlertRule::ruleTypeToString(type).c_str(), name.c_str());
    }
}

std::vector<fty::AlertRule> AlertRuleManager::getRules() const
{
    namespace fs = std::filesystem;

    std::vector<fty::AlertRule> rules;

    for (const auto& entry : m_typePath) {
        const std::string& path = entry.second;

        if (!fs::exists(path)) {
            log_error("Path %s does not exist", path.c_str());
            continue;
        }

        for (auto& f : fs::directory_iterator(path)) {
            if (f.is_regular_file()) {
                if (f.path().extension() == ALERT_RULES_EXT) {
                    log_debug("Found configuration file %s", f.path().c_str());

                    const std::string& filePath = f.path().string();
                    const std::string& fileName = f.path().stem();

                    std::ifstream file;
                    file.open(filePath);

                    std::ostringstream stream;
                    stream << file.rdbuf();

                    rules.push_back(AlertRule(entry.first, fileName, stream.str()));

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
        try {
            storeRule(r);
        } catch (std::invalid_argument& e) {
            log_warning("%s", e.what());
        } catch (std::exception& e) {
            log_error("%s", e.what());
        }
    }
}

void AlertRuleManager::clearRules()
{
    namespace fs = std::filesystem;

    for (const auto& entry : m_typePath) {
        const AlertRule::Type type = entry.first;
        const std::string&    path = entry.second;

        const std::string typeStr = AlertRule::ruleTypeToString(type);

        if (!fs::exists(path)) {
            log_error("Wrong path for type %s", typeStr.c_str());
            continue;
        }

        log_debug("Deleting rules ot type %s ...", typeStr.c_str());
        for (auto& f : fs::directory_iterator(path)) {
            if (f.is_regular_file()) {
                if (f.path().extension() == ALERT_RULES_EXT) {
                    const std::string& name = f.path().stem();

                    auto found =
                        find_if(m_delExclusions.begin(), m_delExclusions.end(), [&](const TypeNamePair& ex) {
                            return ex.first == type && ex.second == name;
                        });

                    if (found != m_delExclusions.end()) {
                        log_warning("Rule %s of type %s set in exclusions: skipping...", name.c_str(),
                            typeStr.c_str());
                        continue;
                    }

                    log_debug("Deleting rule %s:%s ...", typeStr.c_str(), name.c_str());
                    fs::remove(f.path());
                }
            }
        }
    }
}

} // namespace fty
