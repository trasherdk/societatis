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

struct tx_verification_context
{
    bool m_should_be_relayed;
    bool m_verification_failed; // bad tx, should drop connection
    bool m_verifivation_impossible; // the transaction is related with an alternative blockchain
    bool m_added_to_pool;
    bool m_tx_fee_too_small;
};

struct block_verification_context
{
    bool m_added_to_main_chain;
    bool m_verification_failed; // bad block, should drop connection
    bool m_marked_as_orphaned;
    bool m_already_exists;
    bool m_switched_to_alt_chain;
};

} // namespace CryptoNote
