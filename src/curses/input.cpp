#include "input.hpp"
#include "window_paneled.hpp"
#include "stream.hpp"
#include "stream_modifier.hpp"
#include <queue>

namespace curses {
    bool input::read(keyboard_callback _keyboard_callback) {
        int ch = getch();
        return ch == ERR || _keyboard_callback(ch);
    }

    bool input::read(bool process, bool read_mouse) {
        return read([process, read_mouse, this](int ch) {
            bool result = true;

            if (!process)
                return read_mouse ? false : ch == KEY_MOUSE;

            if (read_mouse && ch == KEY_MOUSE) {
                MEVENT e;
				platform::getmouse(&e);
                for (auto& callback_pair : get_enabled_callbacks(mouse_callbacks))
                    result = result ? process_mouse_event(callback_pair, e) : false;
            } else if (ch != KEY_MOUSE) {
                for (auto& callback_pair : get_enabled_callbacks(keyboard_callbacks))
                    result = result ? callback_pair.second(ch) : false;
                if (keyboard_callbacks[nullptr])
                    result = result ? keyboard_callbacks[nullptr](ch) : false;
            }

            return result;
        });
    }

    template <typename T>
    unordered_map<window*, T> input::get_enabled_callbacks(unordered_map<window*, T>& callbacks) {
        unordered_map<window*, T> top_layer_callbacks;
        priority_queue<pair<int, window*>> layers;

        for (auto& callback_pair : callbacks) {
            window* window = callback_pair.first;
            if (window && window->get_paneled_window().get_layer() > 0)
                layers.push(make_pair(window->get_paneled_window().get_layer(), window));
        }

        if (!layers.empty()) {
            int top_layer = layers.top().first;
            while (!layers.empty() && layers.top().first == top_layer) {
                window *window = layers.top().second;
                top_layer_callbacks[window] = callbacks[window];
                layers.pop();
            }
        }

        return top_layer_callbacks;
    }

    bool input::process_mouse_event(mouse_callback_map::value_type& callback_pair, MEVENT e) {
        window* window = callback_pair.first;
        point p;
        stream::get(p, stream::get::coord::BEG)(window);
        e.x -= p.x;
        e.y -= p.y;
        if (e.x >= 0 && e.x < window->get_bounds().width && e.y >= 0 && e.y < window->get_bounds().height)
            return callback_pair.second(e);
        return true;
    }
}