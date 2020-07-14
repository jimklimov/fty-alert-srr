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

#include "messages.h"

namespace fty { namespace alertutils {

    // create response (data is messagebus::UserData)
    messagebus::Message createMessage(const std::string& subject, const std::string& correlationID,
        const std::string& from, const std::string& to, const std::string& status,
        const messagebus::UserData& data)
    {
        messagebus::Message msg;

        if (!subject.empty()) {
            msg.metaData().emplace(messagebus::Message::SUBJECT, subject);
        }
        if (!from.empty()) {
            msg.metaData().emplace(messagebus::Message::FROM, from);
        }
        if (!to.empty()) {
            msg.metaData().emplace(messagebus::Message::TO, to);
        }
        if (!correlationID.empty()) {
            msg.metaData().emplace(messagebus::Message::CORRELATION_ID, correlationID);
        }
        if (!status.empty()) {
            msg.metaData().emplace(messagebus::Message::STATUS, status);
        }

        msg.userData() = data;

        return msg;
    }

}} // namespace fty::alertutils
