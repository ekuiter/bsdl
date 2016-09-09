#include "stream_modifier.hpp"
#include "terminal.hpp"

namespace curses {
    ostream& operator<<(ostream& _stream, const stream::modifier& modifier) {
        stream& curses_stream = terminal::instance().get_stream(_stream);
        modifier(curses_stream.get_window(), &curses_stream);
        return _stream;
    }
}
