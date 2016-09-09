#include "window.hpp"
#include "terminal.hpp"

namespace curses {
    void window::reset_streams() {
        for (auto stream : { &cout, &clog, &cerr })
            if (terminal::instance().get_stream(*stream).get_window() == this)
                terminal::instance().get_stream(*stream).reset_window();
    }

    ostream& operator<<(ostream& _stream, window& _window) {
        stream& curses_stream = terminal::instance().get_stream(_stream);
        curses_stream.set_window(_window);
        return _stream;
    }
}