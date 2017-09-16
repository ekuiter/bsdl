#pragma once

#include "color.hpp"
#include "window.hpp"
#include "rectangle.hpp"
#include "stream.hpp"
#include <iostream>
#include <functional>
#include <curses.h>

using namespace std;

namespace curses {
    class stream::modifier {
    public:
        virtual void operator()(window* window, stream* stream) const = 0;
    };

    class stream::get : public modifier {
    public:
        enum class coord { STD, BEG, MAX, PAR };

    private:
        point& p;
        coord _coord;

    public:
        get(point& _p, coord coord = coord::STD): p(_p), _coord(coord) {}

        void operator()(window* window, stream* = nullptr) const override {
            if (_coord == coord::STD)
                getyx(window::dereference(window), p.y, p.x);
            if (_coord == coord::BEG)
                getbegyx(window::dereference(window), p.y, p.x);
            if (_coord == coord::MAX)
                getmaxyx(window::dereference(window), p.y, p.x);
            if (_coord == coord::PAR)
                getparyx(window::dereference(window), p.y, p.x);
        }
    };

    class stream::step_modifier : public modifier {
        typedef function<int (const rectangle&)> get_coord;
        int length;

    protected:
        void modify(window* window, get_coord get_x, get_coord get_y) const {
            for (int i = 0; i < length; i++) {
                point p, dimensions;
                get((point&) p)(window);
                get(dimensions, get::coord::MAX)(window);
                rectangle bounds(p, dimensions);
                wmove(window::dereference(window), get_y(bounds), get_x(bounds));
            }
        }

    public:
        step_modifier(int _length): length(_length) {}
    };

    class stream::back : public step_modifier {
    public:
        back(int length = 1): step_modifier(length) {}

        void operator()(window* window, stream* = nullptr) const override {
            modify(window,
                   [](const rectangle& bounds) {
                       return bounds.p.x - 1 >= 0 ? bounds.p.x - 1 : bounds.width - 1;
                   }, [](const rectangle& bounds) {
                       return bounds.p.x - 1 >= 0 ? bounds.p.y : bounds.p.y > 0 ? bounds.p.y - 1 : 0;
                   });
        }
    };

    class stream::advance : public step_modifier {
    public:
        advance(int length = 1): step_modifier(length) {}

        void operator()(window* window, stream* = nullptr) const override {
            modify(window,
                   [](const rectangle& bounds) {
                       return bounds.p.x + 1 < bounds.width ? bounds.p.x + 1 : 0;
                   }, [](const rectangle& bounds) {
                       return bounds.p.x + 1 < bounds.width ?
                              bounds.p.y : bounds.p.y + 1 < bounds.height ? bounds.p.y + 1 : 0;
                   });
        }
    };

    class stream::move : public modifier {
        point p;

    public:
        move(const point& _p): p(_p) {}

        void operator()(window* window, stream* = nullptr) const override {
            if (p.y == -1) {
                point tmp_p;
                get((point&) tmp_p)(window);
                wmove(window::dereference(window), tmp_p.y, p.x);
            } else
                wmove(window::dereference(window), p.y, p.x);
        }
    };

    class stream::ext_char : public modifier {
        chtype c;

    public:
        ext_char(chtype _c): c(_c) {}

        void operator()(window*, stream* stream) const override {
            if (stream)
                stream->dstbuf.print(c);
        }
    };

    class stream::write : public modifier {
        string text;
        ext_char c;
        int length;
        bool should_move;

    public:
        write(const string& _text, int _length = 1, bool _should_move = true):
                text(_text), c(0), length(_length), should_move(_should_move) {}

        write(const ext_char& ext_char, int _length = 1, bool _should_move = true):
                text(""), c(ext_char), length(_length), should_move(_should_move) {}

        void operator()(window* window, stream* stream) const override {
            if (stream) {
                point p;
                get((point&) p)(window);
                if (text == "")
                    for (int i = 0; i < length; i++)
                        c(window, stream);
                else
                    for (int i = 0; i < length; i++)
                        *stream << text;
                if (!should_move)
                    move((point&) p)(window);
            }
        }
    };

    class stream::write_centered : public modifier {
        string text;
        int y_offset;

    public:
        write_centered(const string& _text, int _y_offset = 0): text(_text), y_offset(_y_offset) {}

        void operator()(window* window, stream* stream) const override {
            if (stream) {
                rectangle centered_rectangle = rectangle(text.length(), 1).center(
                        window ? window->get_bounds() : rectangle::get_screen()
                );
                centered_rectangle.p.y += y_offset;
                move(centered_rectangle.p)(window);
                *stream << text;
            }
        }
    };

    class stream::write_truncated : public modifier {
        string text;
        int width, postfix_length;

    public:
        write_truncated(const string& _text, int _width): text(_text), width(_width), postfix_length(3) {}

        void operator()(window* window, stream* stream) const override {
            if (stream) {
                point p_before, p_after;
                get((point&) p_before)(window);
                int end = p_before.x + width;

                for (auto c : text) {
                    *stream << c << stream::get(p_after);

                    if (p_after.x > end) {
                        move(end - postfix_length)(window);
                        *stream << "... ";
                        break;
                    }
                }

                move(point(end))(window);
            }
        }
    };
    
    class stream::write_attribute : public modifier {
        chtype attribute;
        color _color;
        int length;

    public:
        write_attribute(chtype _attribute, const color& color, int _length = -1):
                attribute(_attribute), _color(color), length(_length) {}

        void operator()(window* window, stream* = nullptr) const override {
            wchgat(window::dereference(window), length, attribute, _color.get_id(), nullptr);
        }
    };

    class stream::colored : public modifier {
        string text;
        color color;

    public:
        colored(const string& _text, const class color& _color = color::get_accent_color()): text(_text), color(_color) {}

        void operator()(window*, stream* stream) const override {
            if (stream)
                *stream << color << text << color::previous;
        }
    };

    class stream::allow_wrap : public modifier {
    public:
        void operator()(window*, stream* stream) const override {
            if (stream)
                stream->dstbuf.allow_wrap();
        }
    };

    class stream::attribute : public modifier {
        chtype _attribute;

    public:
        attribute(chtype attribute): _attribute(attribute) {}

        void operator()(window*, stream* stream) const override {
            if (stream)
                stream->set_attribute(_attribute);
        }
    };

    class stream::refresh : public modifier {
    public:
        void operator()(window* window, stream* = nullptr) const override {
            wrefresh(window::dereference(window));
        }
    };

    class stream::clear : public modifier {
    public:
        void operator()(window* window, stream* = nullptr) const override {
            wclear(window::dereference(window));
        }
    };

    ostream& operator<<(ostream& stream, const stream::modifier& modifier);
}
