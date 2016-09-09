#pragma once

#include "window.hpp"
#include <panel.h>

using namespace std;

namespace curses {
    class window::paneled : public window {
    protected:
        PANEL* panel;
        int layer_override;

        static void update_panels() {
            ::update_panels();
            doupdate();
        }

        paneled(const rectangle& bounds): window(bounds), layer_override(-1) {}

    public:
        paneled& hide() {
            hide_panel(panel);
            update_panels();
            return *this;
        }

        paneled& show() {
            hide_panel(panel);
            show_panel(panel);
            update_panels();
            return *this;
        }

        paneled& move(point p) {
            bounds.p = p;
            move_panel(panel, p.y, p.x);
            update_panels();
            return *this;
        }

        paneled& get_paneled_window() override {
            return *this;
        }

        paneled& override_layer(int _layer_override) {
            layer_override = _layer_override;
            return *this;
        }

        int get_layer() const {
            int layer = 0;
            PANEL* p = nullptr;
            for (; p != panel; layer++) {
                p = panel_above(p);
                if (p == nullptr)
                    return 0;
            }
            if (layer_override >= 0 && layer != 0)
                return layer_override;
            return layer;
        }
    };
}