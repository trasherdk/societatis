// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2020, The Qwertycoin Group.
// Copyright (c) 2020, Societatis.io
//
// This file is part of Societatis.
//
// Societatis is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Societatis is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Societatis.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <Logging/LoggerRef.h>
#include <System/ContextGroup.h>
#include <System/Dispatcher.h>
#include <System/Event.h>
#include <CryptoTypes.h>

class BlockchainMonitor
{
public:
    BlockchainMonitor(
        System::Dispatcher &dispatcher,
        const std::string &daemonHost,
        uint16_t daemonPort,
        size_t pollingInterval,
        Logging::ILogger &logger
    );

    void waitBlockchainUpdate();
    void stop();

private:
    Crypto::Hash requestLastBlockHash();

private:
    System::Dispatcher &m_dispatcher;
    std::string m_daemonHost;
    uint16_t m_daemonPort;
    size_t m_pollingInterval;
    bool m_stopped;
    System::Event m_httpEvent;
    System::ContextGroup m_sleepingContext;
    Logging::LoggerRef m_logger;
};
