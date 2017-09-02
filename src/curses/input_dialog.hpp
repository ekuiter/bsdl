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
                   text_box** parent_text_box_ptr = nullptr, bool allow_clear = false) {
            stream stream(window);
            point p;
            text_box* text_box_ptr = nullptr;
            stream.set_wrap(true);
            stream << prompt << stream::get(p) << endl;

            int button_height = 3;
            SINGLE_BUTTON(window, action_button, bottom_right, button_height, highlight_color, action,
                          nullptr, input::instance().mouse_event(BUTTON1_PRESSED));

            button_group::button_descriptor clear_button = {
                "Clear", nullptr, input::instance().mouse_event(BUTTON1_PRESSED, [&text_box_ptr]() {
                        text_box_ptr->clear();
                        return true;
                    })
            };

            typedef vector<button_group::button_descriptor> bdv;
            auto button_descriptors = allow_clear ? bdv({ clear_button }) : bdv();
            window::sub clear_wrapper(window, window.bottom_left_rectangle(button_group::get_width(button_descriptors), button_height));
            button_group clear_group(clear_wrapper, button_descriptors, highlight_color);

            window::sub text_box_wrapper(window, rectangle(0, p.y + 1, window.get_bounds().width, 1));
            text_box text_box(text_box_wrapper, highlight_color, initial_text);
            text_box_ptr = &text_box;
            if (parent_text_box_ptr)
                *parent_text_box_ptr = text_box_ptr;

            window.set_keyboard_callback(input::instance().keyboard_event('\n'));
            input::instance().wait();
            return text_box.get_text();
        }
    };
}
