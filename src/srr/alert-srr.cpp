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

#include "alert-srr.h"
#include "src/alert/rule.h"
#include "src/config/config.h"
#include "src/utils/messages.h"
#include "src/utils/serialization.h"
#include <cxxtools/serializationinfo.h>
#include <fty_common_messagebus.h>
#include <fty_log.h>
#include <functional>
#include <map>

namespace fty {

AlertSrr::AlertSrr()
{
}

void AlertSrr::init()
{
    log_debug("Initializing Alert SRR agent");

    using namespace std::placeholders;

    // set alert configuration paths
    for (const auto& entry : getRulesPath()) {
        m_alertRuleManager.addAlertRulesPath(entry);
    }

    // setup SRR client
    m_srrClient.reset(messagebus::MlmMessageBus(ALERT_SRR_ENDPOINT, ALERT_SRR_AGENT));
    log_debug("New client registered to endpoint %s with name %s", ALERT_SRR_ENDPOINT, ALERT_SRR_AGENT);

    m_srrClient->connect();

    m_srrProcessor.saveHandler    = std::bind(&AlertSrr::handleSave, this, _1);
    m_srrProcessor.restoreHandler = std::bind(&AlertSrr::handleRestore, this, _1);
    m_srrProcessor.resetHandler   = std::bind(&AlertSrr::handleReset, this, _1);

    m_srrClient->receive(ALERT_SRR_QUEUE, [&](messagebus::Message m) {
        this->handleSrrRequest(m);
    });
}


void AlertSrr::handleSrrRequest(const messagebus::Message& msg)
{
    log_debug("Processing SRR request");
    using namespace dto;
    using namespace dto::srr;
    using namespace fty::alertutils;

    try {
        UserData data = msg.userData();
        Query    query;
        data >> query;

        messagebus::UserData respData;
        respData << (m_srrProcessor.processQuery(query));

        auto response = alertutils::createMessage(msg.metaData().find(messagebus::Message::SUBJECT)->second,
            msg.metaData().find(messagebus::Message::CORRELATION_ID)->second, ALERT_SRR_AGENT,
            msg.metaData().find(messagebus::Message::REPLY_TO)->second, messagebus::STATUS_OK, respData);

        log_debug(
            "Sending response to: %s", msg.metaData().find(messagebus::Message::REPLY_TO)->second.c_str());
        m_srrClient->sendReply(msg.metaData().find(messagebus::Message::REPLY_TO)->second, response);
    } catch (std::exception& e) {
        log_error("Unexpected error: %s", e.what());
    }
}

dto::srr::SaveResponse AlertSrr::handleSave(const dto::srr::SaveQuery& query)
{
    using namespace dto;
    using namespace dto::srr;
    using namespace fty::alertutils;

    log_debug("Saving alert rules");

    std::map<FeatureName, FeatureAndStatus> mapFeaturesData;

    for (const auto& featureName : query.features()) {
        FeatureAndStatus fs1;
        Feature&         f1 = *(fs1.mutable_feature());

        if (featureName == ALERT_SRR_NAME) {
            f1.set_version(SRR_ACTIVE_VERSION);
            try {
                std::unique_lock<std::mutex> lock(m_srrLock);
                f1.set_data(serializeRules(m_alertRuleManager.getRules(), SRR_ACTIVE_VERSION));
                fs1.mutable_status()->set_status(Status::SUCCESS);
            } catch (std::exception& e) {
                fs1.mutable_status()->set_status(Status::FAILED);
                fs1.mutable_status()->set_error(e.what());
            }

        } else {
            fs1.mutable_status()->set_status(Status::FAILED);
            fs1.mutable_status()->set_error("SRR feature " + featureName + " is not supported");
        }

        mapFeaturesData[featureName] = fs1;
    }

    return (createSaveResponse(mapFeaturesData, SRR_ACTIVE_VERSION)).save();
}

dto::srr::RestoreResponse AlertSrr::handleRestore(const dto::srr::RestoreQuery& query)
{
    using namespace dto;
    using namespace dto::srr;
    using namespace fty::alertutils;

    log_debug("Restoring alert rules");

    std::map<FeatureName, FeatureStatus> mapStatus;

    for (const auto& item : query.map_features_data()) {
        const FeatureName& featureName = item.first;
        const Feature&     feature     = item.second;

        FeatureStatus               featureStatus;
        cxxtools::SerializationInfo data;

        if (featureName == ALERT_SRR_NAME) {
            try {
                std::unique_lock<std::mutex> lock(m_srrLock);
                std::vector<fty::AlertRule>  rules = deserializeRules(feature.data(), SRR_ACTIVE_VERSION);

                featureStatus.set_status(Status::SUCCESS);
            } catch (std::exception& e) {
                featureStatus.set_status(Status::FAILED);
                featureStatus.set_error(e.what());
            }

        } else {
            featureStatus.set_status(Status::FAILED);
            featureStatus.set_error("SRR feature " + featureName + " is not supported");
        }

        mapStatus[featureName] = featureStatus;
    }

    return (createRestoreResponse(mapStatus)).restore();
}

dto::srr::ResetResponse AlertSrr::handleReset(const dto::srr::ResetQuery& query)
{
    using namespace dto;
    using namespace dto::srr;

    log_debug("Resetting alert rules");

    std::map<FeatureName, FeatureStatus> mapStatus;

    log_debug("Received reset query %s", query.DebugString().c_str());

    const FeatureName& featureName = ALERT_SRR_NAME;
    FeatureStatus      featureStatus;
    featureStatus.set_status(Status::FAILED);
    featureStatus.set_error("SRR feature " + featureName + " is not supported");

    mapStatus[featureName] = featureStatus;

    return (createResetResponse(mapStatus)).reset();
}

} // namespace fty
