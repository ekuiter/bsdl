#pragma once

#include "color.hpp"
#include "window.hpp"
#include <iostream>
#include <functional>
#include <curses.h>

using namespace std;

namespace curses {
    class stream : public ostream {
    public:
        class modifier;
        class get;
        class step_modifier;
        class back;
        class advance;
        class move;
        class ext_char;
        class write;
        class write_centered;
        class write_truncated;
        class write_attribute;
        class allow_wrap;
        class attribute;
        class refresh;
        class clear;

    private:
        class cursesbuf : public streambuf {
        protected:
            int attribute;
            window* _window;
            bool visible, refresh, wrap;
            string word;
            ofstream* log_file;

            void print_wrap(chtype c);

        public:
            cursesbuf(): attribute(0), _window(nullptr), visible(true), refresh(true), wrap(false), log_file(nullptr) {}
            cursesbuf(const cursesbuf& other): attribute(other.attribute), _window(other._window), visible(other.visible),
                                               refresh(other.refresh), wrap(other.wrap), log_file(other.log_file) {}

            void print(chtype c);
            virtual int overflow(int c) override;

            void set_attribute(int _attribute) {
                attribute = _attribute;
            }

            window* get_window() {
                return _window;
            }

            void set_window(window* window) {
                _window = window;
            }

            void set_visible(bool _visible) {
                visible = _visible;
            }

            void set_refresh(bool _refresh) {
                refresh = _refresh;
            }

            void set_wrap(bool _wrap) {
                wrap = _wrap;
            }

            void allow_wrap() {
                word = "";
            }

            void set_log_file(ofstream* _log_file) {
                log_file = _log_file;
            }
        };

        ostream* _stream;
        cursesbuf dstbuf;
        streambuf* srcbuf;
        color _color;
        color previous_color;

    public:
        stream(): ostream(&dstbuf), _stream(nullptr), srcbuf(nullptr), _color(COLOR_WHITE), previous_color(COLOR_WHITE) {}

        stream(window& window): stream() {
            set_window(window);
        }

        stream(window& window, const color& color): stream() {
            set_window(window).set_color(color);
        }

        stream(ostream& stream): ostream(&dstbuf), _stream(&stream), srcbuf(stream.rdbuf()),
                                 _color(COLOR_WHITE), previous_color(COLOR_WHITE) {
            _stream->rdbuf(rdbuf());
        }

        stream(stream& other):
                ostream(&dstbuf), _stream(other._stream), dstbuf(other.dstbuf), srcbuf(&other.dstbuf),
                _color(other._color), previous_color(other.previous_color) {
            _stream->rdbuf(rdbuf());
        }

        ~stream() {
            if (_stream)
                _stream->rdbuf(srcbuf);
        }

        stream& set_attribute(int attribute) {
            dstbuf.set_attribute(attribute);
            return *this;
        }

        stream& set_color(const color& color) {
            previous_color = _color;
            _color = color;
            return set_attribute(_color.get());
        }

        color get_color() {
            return _color;
        }

        color get_previous_color() {
            return previous_color;
        }

        window* get_window() {
            return dstbuf.get_window();
        }

        stream& set_window(window& window) {
            dstbuf.set_window(&window);
            return *this;
        }

        stream& reset_window() {
            dstbuf.set_window(nullptr);
            return *this;
        }

        stream& set_visible(bool visible) {
            dstbuf.set_visible(visible);
            return *this;
        }

        stream& set_refresh(bool refresh) {
            dstbuf.set_refresh(refresh);
            return *this;
        }

        stream& set_wrap(bool wrap) {
            dstbuf.set_wrap(wrap);
            return *this;
        }

        stream& set_log_file(ofstream* log_file) {
            dstbuf.set_log_file(log_file);
            return *this;
        }
    };

    ostream& operator<<(ostream& stream, const stream::modifier& back);
}
