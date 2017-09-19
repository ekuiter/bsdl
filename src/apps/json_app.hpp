#pragma once

#include "../app.hpp"

class json_app : public app {
    terminal& terminal;
    settings& settings;
    vector<aggregators::series*> search_results;
    string series_search;
    aggregators::series* current_series = nullptr;
    ofstream log_file;
    
    json_app(): terminal(terminal::instance()), settings(settings::instance()) {}
    void help_message();
    void show_log();
    void run_tests();

public:
    static json_app& instance() {
        if (!_instance)
            _instance.reset(new json_app());
        return dynamic_cast<json_app&>(*_instance);
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
    void set_title(const string& title, bool set_notice = false, string notice = "") override;
    aggregators::series& choose_series(vector<aggregators::series*>& search_results, const string& prompt, const string& action) override;
    vector<aggregators::series*> search_series() override;
    void display_series(aggregators::series& series) override;

    THROW_UNIMPLEMENTED(rectangle, get_centered_bounds, (int width = -1, int height = -1, int quarters = 3));
    THROW_UNIMPLEMENTED(void, download_episodes, (aggregators::download_selection& download_selection));
};
