#pragma once

#include "color.hpp"
#include "stream.hpp"
#include "stream_modifier.hpp"
#include "input.hpp"
#include "window.hpp"
#include "window_sub.hpp"
#include "button.hpp"
#include "menu_vertical.hpp"
#include <iostream>
#include <type_traits>

using namespace std;

namespace curses {
    class menu_dialog {
    public:
        template <typename T>
        static auto run(window& window, const string& prompt, T& pointers,
                        typename remove_reference<decltype(*pointers.begin())>::type selected_ptr = nullptr,
                        const string& action = "Continue", const color& highlight_color = color::get_accent_color())
                        -> typename remove_reference<decltype(*pointers.begin())>::type {

            stream stream(window);
            stream << prompt << endl;

            int button_height = 3;
            SINGLE_BUTTON(window, button, bottom_right, button_height, highlight_color, action,
                          input::instance().keyboard_event('\n'),
                          input::instance().mouse_event(BUTTON1_PRESSED));

            window::sub menu_wrapper(window, window.top_left_rectangle(0, -button_height));
            menu::vertical<T> menu(menu_wrapper, pointers, selected_ptr, highlight_color);

            input::instance().wait();
            return menu.get_selected_pointer();
        }
    };
}
