#pragma once

#include <iostream>
#include "point.hpp"
#include "curses.h"

using namespace std;

namespace curses {
    class rectangle {
    public:
        point p;
        int width, height;

    private:
        rectangle& center(int& coord, int size, int full_size, int margin_before, int margin_after) {
            coord = margin_before + (full_size - margin_before - margin_after - size) / 2;
            return *this;
        }

    public:
        rectangle(): width(0), height(0) {}
        rectangle(int x, int y, int _width, int _height): p(x, y), width(_width), height(_height) {}
        rectangle(const point& _p, int _width, int _height): p(_p), width(_width), height(_height) {}
        rectangle(int x, int y, const point& dimensions): p(x, y), width(dimensions.x), height(dimensions.y) {}
        rectangle(const point& _p, const point& dimensions): p(_p), width(dimensions.x), height(dimensions.y) {}
        rectangle(int _width, int _height): width(_width), height(_height) {}

        point get_dimensions() const {
            return point(width, height);
        }

        rectangle& center_x(const rectangle& full_rectangle, int margin_before = 0, int margin_after = 0) {
            return center(p.x, width, full_rectangle.width, margin_before, margin_after);
        }

        rectangle& center_y(const rectangle& full_rectangle, int margin_before = 0, int margin_after = 0) {
            return center(p.y, height, full_rectangle.height, margin_before, margin_after);
        }

        rectangle& center(const rectangle& full_rectangle, int margin_left = 0,
                          int margin_right = 0, int margin_top = 0, int margin_bottom = 0) {
            center_x(full_rectangle, margin_left, margin_right);
            center_y(full_rectangle, margin_top, margin_bottom);
            return *this;
        }

        static rectangle get_screen() {
            return rectangle(COLS, LINES);
        }
    };
}