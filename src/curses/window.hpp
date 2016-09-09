#pragma once

#include "input.hpp"
#include "rectangle.hpp"
#include "exception.hpp"
#include <iostream>
#include <curses.h>

using namespace std;

namespace curses {
    class window {
    protected:
        WINDOW* content;
        rectangle bounds;

        window(const rectangle& _bounds): bounds(_bounds) {}

        void reset_streams();

        void initialize_content(WINDOW* _content) {
            content = _content;
            if (!content)
                throw exception("window out of bounds");
            keypad(content, true);
            wrefresh(content);
        }

    public:
        class sub;
        class paneled;
        class plain;
        class framed;

        virtual ~window() {
            input::instance().reset_keyboard_callback(*this);
            input::instance().reset_mouse_callback(*this);
        }

        WINDOW* get() const {
            return content;
        }

        const rectangle& get_bounds() const {
            return bounds;
        }

        point get_dimensions() const {
            return bounds.get_dimensions();
        }

        virtual rectangle get_full_bounds() const {
            return bounds;
        }

        window& set_scrolling(bool scrolling) {
            scrollok(content, scrolling);
            return *this;
        }

        window& set_keyboard_callback(input::keyboard_callback _keyboard_callback) {
            input::instance().register_keyboard_callback(*this, _keyboard_callback);
            return *this;
        }

        window& set_mouse_callback(input::mouse_callback _mouse_callback) {
            input::instance().register_mouse_callback(*this, _mouse_callback);
            return *this;
        }

        virtual paneled& get_paneled_window() = 0;

        static WINDOW* dereference(window* window) {
            return window ? window->get() : stdscr;
        }
    };

    ostream& operator<<(ostream& stream, window& window);
}