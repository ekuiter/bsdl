#include <iostream>
#include <memory>
#include <vector>
#include "settings.hpp"
#include "../bs/bs.hpp"
#include "../curses/terminal.hpp"
#include "../curses/widgets.hpp"

using namespace std;
using namespace curses;

class settings;

class app {
    terminal& terminal;
    settings& settings;
    window::framed status_window;
    window::sub title_window;
    point title_height;
    window::sub log_window;
    spinner spinner;
    ofstream log_file;
    bs::series* current_series;

    app();
    void set_title(const string& title);
    bool keyboard_callback(int ch);
    int http_callback(http::request::status status, const http::request& request,
                  curl_off_t now_bytes, curl_off_t total_bytes, curl::curl_easy_exception* e);

public:
    static app& instance() {
        static app instance;
        return instance;
    }

    const bs::series* get_current_series() const {
        return current_series;
    }

    rectangle get_centered_bounds(int width = -1, int height = -1, int thirds = 2) {
        int status_width = status_window.get_full_bounds().width;
        return rectangle(width == -1 ? (COLS - status_width) * thirds / 3 : width,
                  height == -1 ? LINES * thirds / 3 : height).center(rectangle::get_screen(), status_width);
    }

    unique_ptr<vector<bs::series>> search_series();
    bs::series& choose_series(vector<bs::series>& search_results);
    void display_series(bs::series& series);
    void download_episodes(bs::download_selection& download_selection);
};