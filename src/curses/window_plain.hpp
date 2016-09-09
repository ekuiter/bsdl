#pragma once

#include "window_paneled.hpp"

using namespace std;

namespace curses {
    class window::plain : public window::paneled {
    public:
        plain(const rectangle& bounds): window::paneled(bounds) {
            initialize_content(newwin(bounds.height, bounds.width, bounds.p.y, bounds.p.x));
            panel = new_panel(content);
            update_panels();
        }

        plain(const plain& other) = delete;

        ~plain() {
            reset_streams();
            del_panel(panel);
            update_panels();
            delwin(content);
        }
    };
}