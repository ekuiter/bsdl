#include "color.hpp"
#include "terminal.hpp"

namespace curses {
    int color::current_id;
    color color::accent_color;
    color color::previous;

    color::color(ostream &stream): color(terminal::instance().get_stream(stream).get_color()) {}

    ostream& operator<<(ostream& _stream, const color& _color) {
        stream& curses_stream = terminal::instance().get_stream(_stream);
        curses_stream.set_color(&_color == &color::previous ? curses_stream.get_previous_color() : _color);
        return _stream;
    }
}