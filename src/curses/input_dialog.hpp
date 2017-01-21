#pragma once

#include "color.hpp"
#include "stream.hpp"
#include "stream_modifier.hpp"
#include "input.hpp"
#include "window.hpp"
#include "window_sub.hpp"
#include "button.hpp"
#include "text_box.hpp"
#include <iostream>

using namespace std;

namespace curses {
    class input_dialog {
    public:
        static string run(window& window, const string& prompt, const string& action = "Continue",
                   const color& highlight_color = color::get_accent_color(), const string& initial_text = "",
                    text_box** text_box_ptr = nullptr) {
            stream stream(window);
            point p;
            stream.set_wrap(true);
            stream << prompt << stream::get(p) << endl;

            int button_width = action.length() + 4, button_height = 3;
            window::sub button_wrapper(window, rectangle(
                    window.get_dimensions() - point(button_width, button_height), button_width, button_height)),
                    text_box_wrapper(window, rectangle(0, p.y + 1, window.get_bounds().width, 1));
            button button(button_wrapper, action, highlight_color);
            text_box text_box(text_box_wrapper, highlight_color, initial_text);
            if (text_box_ptr)
                *text_box_ptr = &text_box;

            window.set_keyboard_callback(input::instance().keyboard_event('\n'));
            button_wrapper.set_mouse_callback(input::instance().mouse_event(BUTTON1_PRESSED));

            input::instance().wait();
            return text_box.get_text();
        }
    };
}