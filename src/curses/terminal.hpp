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
#include <memory>
#include <curses.h>

using namespace std;

extern int ESCDELAY;

namespace curses {    
    class terminal {
    protected:
        static unique_ptr<terminal> _instance;
    
    public:
        virtual ~terminal() {}
        static terminal& instance(char* locale = setlocale(LC_ALL, NULL));
        virtual void run(function<void (terminal& terminal)> fn) = 0;
        virtual stream& get_stream(ostream& _stream) = 0;
        virtual input& get_input() = 0;
        virtual char* get_locale() = 0;
    };

    class main_terminal : public terminal {
        friend class terminal;
        
        WINDOW* _1;
        int _2;
        stream curses_out, curses_log, curses_err;
        input& _input;
        char* locale;

        main_terminal(char* locale);
        main_terminal(const terminal&) = delete;

    public:
        ~main_terminal() {
            endwin();
        }

        void run(function<void (terminal& terminal)> fn);
        stream& get_stream(ostream& _stream);

        input& get_input() {
            return _input;
        }

        char* get_locale() {
            return locale;
        }
    };
}
