#pragma once

#include "color.hpp"
#include "stream.hpp"
#include "stream_modifier.hpp"
#include "window.hpp"
#include <iostream>

using namespace std;

namespace curses {
    class button {
    public:
        button(window& window, const string& text, const color& highlight_color = color::get_accent_color()) {
            stream _stream(window, highlight_color);
            box(window.get(), 0, 0);
            _stream <<
                    stream::move(rectangle(text.length(), 1).center(window.get_bounds()).p) <<
                    text <<
                    stream::refresh();
        }
    };
}