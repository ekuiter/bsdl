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

            int button_width = action.length() + 4, button_height = 3;
            window::sub button_wrapper(window, rectangle(
                    window.get_dimensions() - point(button_width, button_height), button_width, button_height)),
                    menu_wrapper(window, rectangle(0, 1, window.get_dimensions() - point(0, button_height + 1)));
            button button(button_wrapper, action, highlight_color);
            menu::vertical<T> menu(menu_wrapper, pointers, selected_ptr, highlight_color);

            window.set_keyboard_callback(input::instance().keyboard_event('\n'));
            button_wrapper.set_mouse_callback(input::instance().mouse_event(BUTTON1_PRESSED));

            input::instance().wait();
            return menu.get_selected_pointer();
        }
    };
}