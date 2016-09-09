#pragma once

#include <iostream>
#include <unordered_map>
#include <functional>
#include <curses.h>
#include "exception.hpp"

using namespace std;

namespace curses {
    class window;

    class input {
    public:
        typedef function<bool (int ch)> keyboard_callback;
        typedef function<bool (MEVENT e)> mouse_callback;

    private:
        typedef unordered_map<window*, keyboard_callback> keyboard_callback_map;
        typedef unordered_map<window*, mouse_callback> mouse_callback_map;

        keyboard_callback_map keyboard_callbacks;
        mouse_callback_map mouse_callbacks;
        bool blocking;

        input(): blocking(true) {}

        template <typename T>
        unordered_map<window*, T> get_enabled_callbacks(unordered_map<window*, T>& callbacks);

        bool process_mouse_event(mouse_callback_map::value_type& callback_pair, MEVENT e);

    public:
        static input& instance() {
            static input instance;
            return instance;
        }

        void register_keyboard_callback(keyboard_callback _keyboard_callback) {
            if (keyboard_callbacks[nullptr])
                throw exception("only one global keyboard callback allowed");
            keyboard_callbacks[nullptr] = _keyboard_callback;
        }

        void register_keyboard_callback(window& window, keyboard_callback _keyboard_callback) {
            keyboard_callbacks[&window] = _keyboard_callback;
        }

        void register_mouse_callback(window& window, mouse_callback _mouse_callback) {
            mouse_callbacks[&window] = _mouse_callback;
        }

        void reset_keyboard_callback(window& window) {
            keyboard_callbacks.erase(&window);
        }

        void reset_mouse_callback(window& window) {
            mouse_callbacks.erase(&window);
        }

        keyboard_callback keyboard_event(int _ch, function<bool ()> callback = []() { return false; }) {
            return [_ch, callback](int ch) {
                return ch == _ch ? callback() : true;
            };
        }

        mouse_callback mouse_event(mmask_t _bstate, function<bool ()> callback = []() { return false; }) {
            return [_bstate, callback](MEVENT e) {
                return e.bstate & _bstate ? callback() : true;
            };
        }

        input& set_blocking(bool new_blocking) {
            blocking = new_blocking;
            nodelay(stdscr, !new_blocking);
            return *this;
        }

        input& override_blocking(bool new_blocking) {
            nodelay(stdscr, !new_blocking);
            return *this;
        }

        input& restore_blocking() {
            set_blocking(blocking);
            return *this;
        }

        void wait() {
            while (read());
        }

        void wait_any_key() {
            while (read(false, false));
        }

        bool read(keyboard_callback _keyboard_callback);
        bool read(bool process = true, bool read_mouse = true);
    };
}