#pragma once

#include <stdexcept>

using namespace std;

namespace curses {
    class exception : public runtime_error {

    public:
        exception(const string& msg): runtime_error(msg) {}
    };
}