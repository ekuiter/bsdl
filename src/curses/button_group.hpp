#pragma once

#include "button.hpp"
#include <vector>
#include <tuple>

#define SINGLE_BUTTON(_window, name, corner, height, highlight_color, action, keyboard_callback, mouse_callback) \
    curses::button_group::button_descriptor name = { action, keyboard_callback, mouse_callback }; \
    curses::window::sub name##_wrapper(_window, _window.corner##_rectangle(curses::button_group::get_width({ name }), height)); \
    button_group name##_group(name##_wrapper, { name }, highlight_color);

using namespace std;

namespace curses {
    class button_group {
    public:
        typedef tuple<string, input::keyboard_callback, input::mouse_callback> button_descriptor;

    private:
        window& _window;
        stream _stream;
        color highlight_color;
        int x = 0, button_height;
        vector<window::sub*> button_wrappers;
        vector<button*> buttons;

        static int button_width(const string& action) {
            return action.length() + 4;
        }

        void make_button(const button_descriptor& descriptor) {
            string action;
            input::keyboard_callback _keyboard_callback;
            input::mouse_callback _mouse_callback;
            tie(action, _keyboard_callback, _mouse_callback) = descriptor;
            window::sub* button_wrapper = new window::sub(_window, rectangle(x, 0, button_width(action), button_height));
            button_wrappers.push_back(button_wrapper);
            buttons.push_back(new button(*button_wrapper, action, highlight_color));
            if (x > 0)
                _stream << color(COLOR_WHITE) <<
                    stream::move(point(x, 0)) << stream::ext_char(ACS_TTEE) <<
                    stream::move(point(x, button_height - 1)) << stream::ext_char(ACS_BTEE) << stream::refresh();
            x += button_width(action) - 1;
            if (_keyboard_callback)
                button_wrapper->set_keyboard_callback(_keyboard_callback);
            if (_mouse_callback)
                button_wrapper->set_mouse_callback(_mouse_callback);
        }
        
    public:
        button_group(window& window, const vector<button_descriptor>& button_descriptors, const color& _highlight_color = color::get_accent_color()):
            _window(window), _stream(window), highlight_color(_highlight_color), button_height(window.get_bounds().height) {

            for (auto descriptor : button_descriptors)
                make_button(descriptor);
        }

        ~button_group() {
            for (auto button : buttons)
                delete button;
            for (auto button_wrapper : button_wrappers)
                delete button_wrapper;
        }
        
        static int get_width(const vector<button_group::button_descriptor>& button_descriptors) {
            int width = 0;
            for (auto descriptor : button_descriptors)
                width += button_width(get<0>(descriptor));
            width -= button_descriptors.size() - 1;
            return width;
        }
    };
}
