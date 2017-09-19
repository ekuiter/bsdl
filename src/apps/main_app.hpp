#pragma once

#include "curses_app.hpp"

class main_app : public curses_app {
    friend class app;
    
    vector<aggregators::series*> search_results;
    aggregators::series* current_series = nullptr;
    string title, series_search;

    main_app() {}
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

    bool is_testing() override {
        return false;
    }
    
    void initialize() override;
    vector<aggregators::series*> search_series() override;
    aggregators::series& choose_series(vector<aggregators::series*>& search_results,
       const string& prompt = "The following series were found:", const string& action = "Choose") override;
    void display_series(aggregators::series& series) override;
    void download_episodes(aggregators::download_selection& download_selection) override;
};
