#pragma once

using namespace std;

namespace curses {
    struct point {
        int x, y;

        point(): x(0), y(0) {}
        point(int _x): x(_x), y(-1) {}
        point(int _x, int _y): x(_x), y(_y) {}

        point operator+(const point& other) const {
            return point(x + other.x, y + other.y);
        }

        point& operator+=(const point& other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        point operator-(const point& other) const {
            return point(x - other.x, y - other.y);
        }

        point& operator-=(const point& other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }
    };
}