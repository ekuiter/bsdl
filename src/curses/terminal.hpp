#pragma once

#include "exception.hpp"
#include "rectangle.hpp"
#include "color.hpp"
#include "window.hpp"
#include "window_paneled.hpp"
#include "window_sub.hpp"
#include "window_framed.hpp"
#include "window_plain.hpp"
#include "stream.hpp"
#include "stream_modifier.hpp"
#include "input.hpp"
#include <iostream>
#include <curses.h>

using namespace std;

extern int ESCDELAY;

namespace curses {
    class terminal {
        WINDOW* _1;
        int _2;
        stream curses_out, curses_log, curses_err;
        input& _input;

        terminal();
        terminal(const terminal&) = delete;

    public:
        static terminal& instance() {
            static terminal instance;
            return instance;
        }

        ~terminal() {
            endwin();
        }

        void run(function<void (terminal& terminal)> fn);
        stream& get_stream(ostream& _stream);

        input& get_input() {
            return _input;
        }
    };
}
