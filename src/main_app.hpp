#pragma once

#include "app.hpp"

class main_app : public app {
    friend class app;
    
    terminal& terminal;
    settings& settings;
    window::framed status_window;
    window::sub title_window;
    point title_height;
    window::sub log_window;
    spinner spinner;
    ofstream log_file;
    vector<aggregators::series*> search_results;
    aggregators::series* current_series;
    string title, series_search;

    main_app();
    bool keyboard_callback(int ch);
    int http_callback(http::request::status status, const http::request& request,
                  curl_off_t now_bytes, curl_off_t total_bytes, curl::curl_easy_exception* e);
    void version_message();
    string run_start_window(const rectangle& bounds);

public:
    const vector<aggregators::series*>& get_search_results() const override {
        return search_results;
    }
    
    const aggregators::series* get_current_series() const override {
        return current_series;
    }
    
    const string& get_series_search() const override {
        return series_search;
    }
    
    void set_current_series(aggregators::series& series) override {
        current_series = &series;
    }

    rectangle get_centered_bounds(int width = -1, int height = -1, int quarters = 3) override {
        int status_width = status_window.get_full_bounds().width;
        return rectangle(width == -1 ? (COLS - status_width) * quarters / 4 : width,
                  height == -1 ? LINES * quarters / 4 : height).center(rectangle::get_screen(), status_width);
    }

    bool is_testing() override {
        return false;
    }
    
    void initialize() override;
    void set_title(const string& title, bool set_notice = false, string notice = "") override;
    vector<aggregators::series*> search_series() override;
    aggregators::series& choose_series(vector<aggregators::series*>& search_results,
       const string& prompt = "The following series were found:", const string& action = "Choose") override;
    void display_series(aggregators::series& series) override;
    void download_episodes(aggregators::download_selection& download_selection) override;
};
