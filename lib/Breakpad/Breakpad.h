// Copyright (c) 2018-2020, The Qwertycoin Group.
// Copyright (c) 2020-2021, Societatis.io
//
// Licensed under the GNU General Public License, Version 3.
// See the file LICENSE from this package for details.

#pragma once

#include <string>

namespace google_breakpad {
class ExceptionHandler;
} // namespace google_breakpad

namespace Societatis {

namespace Breakpad {

class ExceptionHandler
{
public:
    explicit ExceptionHandler(const std::string &dumpPath = std::string());
    virtual ~ExceptionHandler();

    static void dummyCrash();

private:
    google_breakpad::ExceptionHandler *m_exceptionHandler = nullptr;
};

} // namespace Breakpad

} // namespace Societatis
