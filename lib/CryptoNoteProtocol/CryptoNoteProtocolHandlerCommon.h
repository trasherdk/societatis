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

namespace CryptoNote {

struct NOTIFY_NEW_BLOCK_request;
struct NOTIFY_NEW_TRANSACTIONS_request;

struct i_cryptonote_protocol
{
    virtual void relay_block(NOTIFY_NEW_BLOCK_request &arg) = 0;
    virtual void relay_transactions(NOTIFY_NEW_TRANSACTIONS_request &arg) = 0;
};

struct cryptonote_protocol_stub : public i_cryptonote_protocol
{
    void relay_block(NOTIFY_NEW_BLOCK_request &arg) override {}
    void relay_transactions(NOTIFY_NEW_TRANSACTIONS_request &arg) override {}
};

} // namespace CryptoNote
