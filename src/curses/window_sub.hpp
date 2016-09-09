#pragma once

#include "window.hpp"

using namespace std;

namespace curses {
    class window::sub : public window {
        window& parent;

    public:
        sub(window& _parent, const rectangle& bounds): window(bounds), parent(_parent) {
            initialize_content(derwin(parent.get(), bounds.height, bounds.width, bounds.p.y, bounds.p.x));
        }

        sub(const sub& other) = delete;

        paneled& get_paneled_window() override {
            try {
                return dynamic_cast<paneled&>(parent);
            } catch (bad_cast) {
                return dynamic_cast<sub&>(parent).get_paneled_window();
            }
        }

        ~sub() {
            reset_streams();
            delwin(content);
        }
    };
}