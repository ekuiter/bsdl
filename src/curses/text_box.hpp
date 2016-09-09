#pragma once

#include "color.hpp"
#include "stream.hpp"
#include "stream_modifier.hpp"
#include "window.hpp"
#include <iostream>

using namespace std;

namespace curses {
    class text_box {
        window& _window;
        stream _stream;
        color highlight_color;
        string text;
        int cursor;

        void refresh() {
            curs_set(0);
            point p;
            _stream << stream::move(point(0, 0)) << text <<
                    stream::get(p) << stream::write(" ", _window.get_bounds().width - p.x) <<
                    stream::move(point(cursor, 0)) << stream::refresh();
            curs_set(1);
        }

    public:
        text_box(window& window, const color& _highlight_color = color::get_accent_color()):
                _window(window), _stream(window), highlight_color(_highlight_color), cursor(0) {
            window.set_keyboard_callback([this](int ch) {
                if (ch >= 0x20 && ch <= 0x7e && text.length() < _window.get_bounds().width - 1) // printable ASCII
                    text.insert(cursor++, 1, ch);
                if (ch == 0x7f && cursor > 0) // backspace
                    text.erase(--cursor, 1);
                if (ch == KEY_LEFT && cursor > 0)
                    cursor--;
                if (ch == KEY_RIGHT && cursor < text.length())
                    cursor++;
                refresh();
                return true;
            });

            window.set_mouse_callback([this](MEVENT e) {
                if (e.bstate & BUTTON1_PRESSED)
                    cursor = e.x < text.length() ? e.x : text.length();
                refresh();
                return true;
            });

            _stream << stream::attribute(A_REVERSE | highlight_color.get());
            refresh();
        }

        ~text_box() {
            curs_set(0);
        }

        const string& get_text() {
            return text;
        }
    };
}