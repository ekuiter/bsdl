#pragma once

#include "app.hpp"

class curses_app : public app {
    friend class app;

protected:
    terminal& terminal;
    settings& settings;
    window::framed status_window;
    window::sub title_window;
    point title_height;
    window::sub log_window;
    spinner spinner;
    ofstream log_file;
    string title;

    curses_app();
    bool keyboard_callback(int ch);
    int http_callback(http::request::status status, const http::request& request,
                      curl_off_t now_bytes, curl_off_t total_bytes, curl::curl_easy_exception* e);

public:
    void set_title(const string& title, bool set_notice = false, string notice = "") override;

    rectangle get_centered_bounds(int width = -1, int height = -1, int quarters = 3) override {
        int status_width = status_window.get_full_bounds().width;
        return rectangle(width == -1 ? (COLS - status_width) * quarters / 4 : width,
                  height == -1 ? LINES * quarters / 4 : height).center(rectangle::get_screen(), status_width);
    }
};
