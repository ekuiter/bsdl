#pragma once

#include "test_util.hpp"
#include "../app.hpp"

struct mock_app : public app {
    string series_search;
    aggregators::series* current_series;
    
    struct mock_error : public runtime_error {
        mock_error(const string& msg): runtime_error(msg + " called on mock app") {}
    };
    
    mock_app() {}

    static mock_app& instance() {
        if (!_instance)
            _instance.reset(new mock_app());
        return dynamic_cast<mock_app&>(*_instance);
    }

    bool is_testing() override {
        return true;
    }

    const string& get_series_search() const override {
        return series_search;
    }

    const aggregators::series* get_current_series() const override {
        return current_series;
    }

    aggregators::series& choose_series(vector<aggregators::series*>& search_results, const string& prompt, const string& action) override {
        return *search_results[0];
    }

    MOCK_THROW(const vector<aggregators::series*>&, get_search_results, () const);
    MOCK_THROW(void, set_current_series, (aggregators::series& series));
    MOCK_THROW(rectangle, get_centered_bounds, (int width = -1, int height = -1, int quarters = 3));
    MOCK_THROW(void, initialize, ());
    MOCK_THROW(void, help_message, ());
    MOCK_THROW(void, version_message, ());
    MOCK_EMPTY(void, set_title, (const string& title, bool set_notice = false, string notice = ""));
    MOCK_THROW(string, run_start_window, (const rectangle& bounds));
    MOCK_THROW(vector<aggregators::series*>, search_series, ());
    MOCK_THROW(void, display_series, (aggregators::series& series));
    MOCK_THROW(void, download_episodes, (aggregators::download_selection& download_selection));
};
