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
        virtual void run(function<void ()> fn) = 0;
        virtual stream& get_stream(ostream& _stream) = 0;
        virtual input& get_input() = 0;
        virtual char* get_locale() = 0;
        virtual void with_output(ostream& _stream, function<void ()> fn) = 0;
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
        static terminal& reset_instance(char* locale = setlocale(LC_ALL, NULL)) {
            _instance.reset(nullptr);
            return instance(locale);
        }
        
        ~main_terminal() {
            endwin();
        }

        void run(function<void ()> fn) override;
        stream& get_stream(ostream& _stream) override;

        input& get_input() override {
            return _input;
        }

        char* get_locale() override {
            return locale;
        }

        void with_output(ostream&, function<void ()>) override {
            throw runtime_error("calling unimplemented main_terminal::with_output");
        }
    };

    struct plain_terminal : public terminal {
        streambuf* old_out, *old_log, *old_err;
        stream out, log, err;

        plain_terminal(): old_out(cout.rdbuf()), old_log(clog.rdbuf()), old_err(cerr.rdbuf()),
                          out(cout), log(clog), err(cerr) {}

        static terminal& instance() {
            if (!_instance)
                _instance.reset(new plain_terminal());
            return *_instance;
        }

        void run(function<void ()> fn) override {
            try {
                fn();
            } catch (const std::exception& e) {
                with_output(cerr, [&e]() {
                        cerr << "unhandled exception of type "
                             << typeid(e).name() << ":" << endl << e.what() << endl;
                    });
                exit(EXIT_FAILURE);
            }
        }

        void with_output(ostream& _stream, function<void ()> fn) override {
            streambuf* buf = _stream.rdbuf();
            _stream.rdbuf(get_old_buf(_stream));
            fn();
            _stream.rdbuf(buf);
        }
        
        input& get_input() override {
            throw runtime_error("calling unimplemented plain_terminal::get_input");
        }
        
        char* get_locale() override {
            throw runtime_error("calling unimplemented plain_terminal::get_locale");
        }

        stream& get_stream(ostream& _stream) override {
            if (&_stream == &cout)
                return out;
            if (&_stream == &clog)
                return log;
            if (&_stream == &cerr)
                return err;
            try {
                return dynamic_cast<stream&>(_stream);
            } catch (bad_cast) {
                throw exception("there is no such plain stream");
            }
        }

        streambuf* get_old_buf(ostream& _stream) {
            if (&_stream == &cout)
                return old_out;
            else if (&_stream == &clog)
                return old_log;
            else if (&_stream == &cerr)
                return old_err;
            else
                throw exception("there is no such plain stream");
        }
    };
}
