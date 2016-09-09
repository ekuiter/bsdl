#pragma once

#include "color.hpp"
#include "window.hpp"
#include "stream.hpp"
#include "stream_modifier.hpp"
#include <iostream>
#include <vector>
#include <curses.h>

using namespace std;

namespace curses {
    class spinner {
        window& _window;
        stream _stream;
        string start, stopped, end;
        vector<string> sequence;
        color running_color, success_color, failure_color;
        int sequence_index;

    public:
        spinner(window& window,
                const string& _start = "[", const vector<string>& _sequence = { "/", "-", "\\", "|" },
                const string& _stopped = "*", const string& _end = "] ",
                const color& _running_color = color(COLOR_YELLOW), const color& _success_color = color(COLOR_GREEN),
                const color& _failure_color = color(COLOR_RED)):
                _window(window), _stream(window), start(_start), stopped(_stopped), end(_end), sequence(_sequence),
                running_color(_running_color),  success_color(_success_color), failure_color(_failure_color),
                sequence_index(0) {
            _stream.set_wrap(true);
        }

        void spin() {
            _stream << running_color <<
                    start <<
                    sequence[sequence_index = (sequence_index + 1) % sequence.size()] <<
                    end << stream::allow_wrap() << stream::back(length());
        }

        void stop(bool success) {
            _stream << color(success ? success_color : failure_color) << start << stopped << end << stream::allow_wrap();
            sequence_index = 0;
        }

        int length() {
            return start.length() + stopped.length() + end.length();
        }
    };
}