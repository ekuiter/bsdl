#pragma once

#include <iostream>
#include <curses.h>

using namespace std;

namespace curses {
    class color {
        static int current_id;
        static color accent_color;
        int id, fg, bg;

        color(): id(-1), fg(-1), bg(-1) {}

    public:
        static color previous;

        color(ostream& stream);

        color(int _fg, int _bg = COLOR_BLACK): id(++current_id), fg(_fg), bg(_bg) {
            init_pair(id, fg, bg);
        }

        int get_id() const {
            return id;
        }

        int get() const {
            return COLOR_PAIR(id);
        }

        static color get_accent_color() {
            return accent_color;
        }

        static void set_accent_color(const color& _accent_color) {
            accent_color = _accent_color;
        }
    };

    ostream& operator<<(ostream& stream, const color& color);
}
