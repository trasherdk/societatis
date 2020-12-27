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

#include <string>
#include <system_error>

namespace CryptoNote {

namespace error {

// custom error conditions enum type:
enum NodeErrorCodes
{
    NOT_INITIALIZED = 1,
    ALREADY_INITIALIZED,
    NETWORK_ERROR,
    NODE_BUSY,
    INTERNAL_NODE_ERROR,
    REQUEST_ERROR,
    CONNECT_ERROR
};

// custom category:
class NodeErrorCategory : public std::error_category
{
public:
    static NodeErrorCategory INSTANCE;

    const char *name() const noexcept override
    {
        return "NodeErrorCategory";
    }

    std::error_condition default_error_condition(int ev) const noexcept override
    {
        return std::error_condition{ev, *this};
    }

    std::string message(int ev) const override
    {
        switch (ev) {
        case NOT_INITIALIZED:
            return "Object was not initialized";
        case ALREADY_INITIALIZED:
            return "Object has been already initialized";
        case NETWORK_ERROR:
            return "Network error";
        case NODE_BUSY:
            return "Node is busy";
        case INTERNAL_NODE_ERROR:
            return "Internal node error";
        case REQUEST_ERROR:
            return "Error in request parameters";
        case CONNECT_ERROR:
            return "Can't connect to daemon";
        default:
            return "Unknown error";
        }
    }

private:
    NodeErrorCategory() = default;
};

} // namespace error

} // namespace CryptoNote

inline std::error_code make_error_code(CryptoNote::error::NodeErrorCodes e)
{
    return std::error_code{static_cast<int>(e), CryptoNote::error::NodeErrorCategory::INSTANCE};
}
