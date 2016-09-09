#pragma once

#include "window_paneled.hpp"

using namespace std;

namespace curses {
    class window::framed : public window::paneled {
        WINDOW* container;

    public:
        framed(const rectangle& _bounds, int width_padding = 1, int height_padding = 0):
                window::paneled(_bounds) {
            container = newwin(bounds.height, bounds.width, bounds.p.y, bounds.p.x);
            bounds.p += point(1 + width_padding, 1 + height_padding);
            bounds.width -= 2 * (1 + width_padding);
            bounds.height -= 2 * (1 + height_padding);
            initialize_content(derwin(container, bounds.height, bounds.width, 1 + height_padding, 1 + width_padding));
            box(container, 0, 0);
            wrefresh(container);
            panel = new_panel(container);
            update_panels();
        }

        framed(const framed& other) = delete;

        ~framed() {
            reset_streams();
            del_panel(panel);
            update_panels();
            delwin(content);
            delwin(container);
        }

        virtual rectangle get_full_bounds() const override {
            rectangle full_bounds = bounds;
            getbegyx(container, full_bounds.p.y, full_bounds.p.x);
            getmaxyx(container, full_bounds.height, full_bounds.width);
            return full_bounds;
        }
    };
}