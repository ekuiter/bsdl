#include "window.hpp"
#include "terminal.hpp"

#define DEFAULT_WIDTH  bounds.width
#define DEFAULT_HEIGHT bounds.height
#define TOP    0
#define LEFT   0
#define BOTTOM (bounds.height - rectangle_height)
#define RIGHT  (bounds.width - rectangle_width)

#define CORNER_RECTANGLE(corner, tb, lr)                                \
    rectangle window::corner##_rectangle(int rectangle_width, int rectangle_height) { \
        rectangle_width = rectangle_width == 0 ? rectangle_width = DEFAULT_WIDTH : rectangle_width; \
        rectangle_height = rectangle_height == 0 ? rectangle_height = DEFAULT_HEIGHT : rectangle_height; \
        if (rectangle_width < 0) rectangle_width = DEFAULT_WIDTH + rectangle_width; \
        if (rectangle_height < 0) rectangle_height = DEFAULT_HEIGHT + rectangle_height; \
        return rectangle(lr, tb, rectangle_width, rectangle_height);    \
    }

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

    CORNER_RECTANGLE(top_left, TOP, LEFT)
    CORNER_RECTANGLE(top_right, TOP, RIGHT)
    CORNER_RECTANGLE(bottom_left, BOTTOM, LEFT)
    CORNER_RECTANGLE(bottom_right, BOTTOM, RIGHT)
}
