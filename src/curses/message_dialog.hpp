#pragma once

#include "color.hpp"
#include "stream.hpp"
#include "stream_modifier.hpp"
#include "input.hpp"
#include "window.hpp"
#include "window_sub.hpp"
#include "button_group.hpp"
#include <iostream>
#include <functional>

using namespace std;

namespace curses {
    class message_dialog {
    public:
        static void run(window& window, function<void (stream&)> fn, const string& action = "Continue",
                        const color& highlight_color = color::get_accent_color()) {
            stream stream(window);
            stream.set_wrap(true);
            if (fn)
                fn(stream);

            SINGLE_BUTTON(window, button, bottom_right, 3, highlight_color, action,
                          input::instance().keyboard_event('\n'),
                          input::instance().mouse_event(BUTTON1_PRESSED));

            input::instance().wait();
        }

        static void run(window& window, const string& message, const string& action = "Continue",
                        const color& highlight_color = color::get_accent_color()) {
            run(window, [&message](stream& _stream) {
                _stream << message;
            }, action, highlight_color);
        }
    };
}
