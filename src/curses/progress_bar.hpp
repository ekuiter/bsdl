#pragma once

#include "color.hpp"
#include "window.hpp"
#include "stream.hpp"
#include "stream_modifier.hpp"
#include <iostream>
#include <curses.h>

using namespace std;

namespace curses {
    class progress_bar {
        static string times(string str, int times) {
            string ret;
            for (int i = 0; i < times; i++)
                ret += str;
            return ret;
        }

    public:
        static void write(ostream& stream, int width, double percentage, string text,
                          color highlight_color = color::get_accent_color()) {
            int padding_left = (width - 2 - text.length()) / 2,
                    padding_right = width - 2 - text.length() - padding_left;
            text = times(" ", padding_left) + text + times(" ", padding_right);
            int highlight_length = percentage * text.length();

            point p;
            stream << highlight_color << "[" << stream::get(p) << text << "]" << stream::move(p) <<
                    stream::write_attribute(A_REVERSE, highlight_color, highlight_length) <<
                    stream::back(1) << color::previous;
        }
    };
}