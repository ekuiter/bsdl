#pragma once

#include <iostream>
#include "../curses/terminal.hpp"
#include "../curses/widgets.hpp"

using namespace std;

namespace bs {
    class season;

    class season_view {
        unique_ptr<curses::window::sub> menu_wrapper, start_download_wrapper,
                download_all_wrapper, download_selected_wrapper;
        unique_ptr<curses::menu::vertical<season>> menu;
        unique_ptr<curses::button> start_download, download_all, download_selected;

    public:
        season_view(season& season, curses::window& window);
    };
}
