#pragma once

#include <iostream>
#include "../curses/terminal.hpp"
#include "../curses/widgets.hpp"
#include "../util/with_map_range.hpp"

using namespace std;
    
namespace aggregators {
    class season_base;
    typedef util::with_map_range<season_base> season;

    class season_view {
        unique_ptr<curses::window::sub> menu_wrapper;
        unique_ptr<curses::window::sub> buttons_wrapper;
        unique_ptr<curses::menu::vertical<season>> menu;
        unique_ptr<curses::button_group> buttons;

    public:
        season_view(season& season, curses::window& window);
    };
}
