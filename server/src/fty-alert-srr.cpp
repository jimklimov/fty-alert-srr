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

#include "srr/alert-srr.h"
#include <csignal>
#include <fty_common_messagebus.h>
#include <fty_log.h>
#include <fty_srr_dto.h>
#include <iostream>
#include <mutex>

static constexpr const char* AGENT_NAME = "fty-alert-srr";

volatile bool           g_exit = false;
std::condition_variable g_cv;
std::mutex              g_cvMutex;

void sigHandler(int)
{
    g_exit = true;
    g_cv.notify_one();
}

void setSignalHandler()
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sigHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}

void terminateHandler()
{
    log_error((AGENT_NAME + std::string(" Error")).c_str());
    exit(EXIT_FAILURE);
}

int main()
{
    setSignalHandler();
    std::set_terminate(terminateHandler);
    ftylog_setInstance(AGENT_NAME, "");

    fty::AlertSrr agent;

    log_info((AGENT_NAME + std::string(" starting")).c_str());

    agent.init();

    log_info((AGENT_NAME + std::string(" started")).c_str());

    std::unique_lock<std::mutex> lock(g_cvMutex);
    g_cv.wait(lock, [] { return g_exit; });

    log_info((AGENT_NAME + std::string(" stopped")).c_str());

    return EXIT_SUCCESS;
}
