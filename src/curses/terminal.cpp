#include "terminal.hpp"
#include "message_dialog.hpp"

namespace curses {
    unique_ptr<terminal> terminal::_instance = nullptr;

    terminal& terminal::instance() {
        if (!_instance)
            _instance.reset(new main_terminal());
        return *_instance;
    }
    
    main_terminal::main_terminal(): locale(setlocale(LC_ALL, "")), _1(initscr()), _2(start_color()),
                                    curses_out(cout), curses_log(clog), curses_err(cerr), _input(input::instance()) {
        color::set_accent_color(color(COLOR_CYAN));
        raw();
        noecho();
        keypad(stdscr, true);
        scrollok(stdscr, true);
        platform::curs_set(0);
        mousemask(ALL_MOUSE_EVENTS, 0);
        mouseinterval(0);
		platform::set_escdelay(0);
        refresh();
    }

    void main_terminal::run(function<void (terminal& terminal)> fn) {
        try {
            if (COLS < 80 || LINES < 15)
                throw exception("the terminal is too small, please resize your window!");
            fn(*this);
        } catch (const std::exception& e) {
            window::plain screen_window(rectangle::get_screen());
            window::framed error_window(rectangle(COLS / 3 * 2, LINES / 3 * 2).center(rectangle::get_screen()));

            message_dialog::run(error_window, [this, &e, &error_window](stream& _stream) {
                get_stream(cerr).set_window(error_window).set_visible(true);
                cerr << color(COLOR_RED) << "unhandled exception of type "
                     << typeid(e).name() << ":" << endl << e.what() << endl;
            }, "Exit", color(COLOR_RED));

            exit(EXIT_FAILURE);
        }
    }

    stream& main_terminal::get_stream(ostream& _stream) {
        if (&_stream == &cout)
            return curses_out;
        if (&_stream == &clog)
            return curses_log;
        if (&_stream == &cerr)
            return curses_err;
        try {
            return dynamic_cast<stream&>(_stream);
        } catch (bad_cast) {
            throw exception("there is no such curses stream");
        }
    }
}
