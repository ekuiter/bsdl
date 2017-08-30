#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include "settings.hpp"
#include "aggregators/bs/bs.hpp"
#include "aggregators/kx/kx.hpp"
#include "curses/terminal.hpp"
#include "curses/widgets.hpp"
#include "util/with_range.hpp"

#define THROW_UNIMPLEMENTED(return_type, fn, signature)         \
    return_type fn signature override {                         \
        throw runtime_error("calling unimplemented " #fn);      \
    }

using namespace std;
using namespace curses;

class app {
protected:
    static unique_ptr<app> _instance;
    
public:
    static app& instance();
    virtual const vector<aggregators::series*>& get_search_results() const = 0;
    virtual const aggregators::series* get_current_series() const = 0;
    virtual const string& get_series_search() const = 0;
    virtual void set_current_series(aggregators::series& series) = 0;
    virtual rectangle get_centered_bounds(int width = -1, int height = -1, int quarters = 3) = 0;
    virtual void initialize() = 0;
    virtual void help_message() = 0;
    virtual void version_message() = 0;
    virtual void set_title(const string& title, bool set_notice = false, string notice = "") = 0;
    virtual string run_start_window(const rectangle& bounds) = 0;
    virtual vector<aggregators::series*> search_series() = 0;
    virtual aggregators::series& choose_series(vector<aggregators::series*>& search_results,
        const string& prompt = "The following series were found:", const string& action = "Choose") = 0;
    virtual void display_series(aggregators::series& series) = 0;
    virtual void download_episodes(aggregators::download_selection& download_selection) = 0;
    virtual bool is_testing() = 0;
};

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
    void version_message() override;
    void set_title(const string& title, bool set_notice = false, string notice = "") override;
    string run_start_window(const rectangle& bounds) override;
    vector<aggregators::series*> search_series() override;
    aggregators::series& choose_series(vector<aggregators::series*>& search_results,
       const string& prompt = "The following series were found:", const string& action = "Choose") override;
    void display_series(aggregators::series& series) override;
    void download_episodes(aggregators::download_selection& download_selection) override;
    
    THROW_UNIMPLEMENTED(void, help_message, ());
};

class batch_app : public app {
    terminal& terminal;
    settings& settings;
    vector<aggregators::series*> search_results;
    string series_search;
    aggregators::series* current_series = nullptr;
    ofstream log_file;
    
    batch_app(): terminal(terminal::instance()), settings(settings::instance()) {}

public:
    static batch_app& instance() {
        if (!_instance)
            _instance.reset(new batch_app());
        return dynamic_cast<batch_app&>(*_instance);
    }

    bool is_testing() override {
        return false;
    }

    const vector<aggregators::series*>& get_search_results() const override {
        return search_results;
    }

    const string& get_series_search() const override {
        return series_search;
    }

    const aggregators::series* get_current_series() const override {
        return current_series;
    }

    void set_current_series(aggregators::series& series) override {
        current_series = &series;
    }

    void initialize() override;
    void help_message() override;
    void show_log();
    void run_tests();
    void set_title(const string& title, bool set_notice = false, string notice = "") override;
    aggregators::series& choose_series(vector<aggregators::series*>& search_results, const string& prompt, const string& action) override;
    vector<aggregators::series*> search_series() override;
    void display_series(aggregators::series& series) override;

    THROW_UNIMPLEMENTED(rectangle, get_centered_bounds, (int width = -1, int height = -1, int quarters = 3));
    THROW_UNIMPLEMENTED(void, version_message, ());
    THROW_UNIMPLEMENTED(string, run_start_window, (const rectangle& bounds));
    THROW_UNIMPLEMENTED(void, download_episodes, (aggregators::download_selection& download_selection));
};
