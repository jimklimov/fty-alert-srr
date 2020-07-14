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

#include "src/alert/rule_manager.h"
#include <fty_srr_dto.h>
#include <vector>

namespace messagebus {
class Message;
class MessageBus;
} // namespace messagebus

namespace fty {

// agent constants
static constexpr const char* ALERT_SRR_ENDPOINT = "ipc://@/malamute";
static constexpr const char* ALERT_SRR_AGENT    = "alert-agent-srr";
static constexpr const char* ALERT_SRR_QUEUE    = "FTY.Q.ALERT.SRR";

// SRR constants
static constexpr const char* ALERT_SRR_NAME     = "alert-agent";
static constexpr const char* SRR_ACTIVE_VERSION = "1.0";

class AlertSrr
{
public:
    AlertSrr();
    ~AlertSrr() = default;

    void init();

private:
    using MsgBusPtr = std::unique_ptr<messagebus::MessageBus>;

    // SRR
    MsgBusPtr                   m_srrClient;
    dto::srr::SrrQueryProcessor m_srrProcessor;
    std::mutex                  m_srrLock;

    // alert rules
    fty::AlertRuleManager m_alertRuleManager;

    void                      handleSrrRequest(const messagebus::Message& msg);
    dto::srr::SaveResponse    handleSave(const dto::srr::SaveQuery& query);
    dto::srr::RestoreResponse handleRestore(const dto::srr::RestoreQuery& query);
    dto::srr::ResetResponse   handleReset(const dto::srr::ResetQuery& query);
};

} // namespace fty
