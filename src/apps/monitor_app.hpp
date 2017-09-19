#pragma once

#include "curses_app.hpp"

class monitor_app : public curses_app {
    aggregators::series* current_series = nullptr;
    
    monitor_app() {}

public:
    static monitor_app& instance() {
        if (!_instance)
            _instance.reset(new monitor_app());
        return dynamic_cast<monitor_app&>(*_instance);
    }

    bool is_testing() override {
        return false;
    }

    const aggregators::series* get_current_series() const override {
        if (!current_series)
            throw runtime_error("there is no current series");
        return current_series;
    }
    
    void set_current_series(aggregators::series& series) override {
        current_series = &series;
    }

    void initialize() override;

    THROW_UNIMPLEMENTED(vector<aggregators::series*>&, get_search_results, () const);
    THROW_UNIMPLEMENTED(string&, get_series_search, () const);
    THROW_UNIMPLEMENTED(aggregators::series&, choose_series, (vector<aggregators::series*>& search_results, const string& prompt, const string& action));
    THROW_UNIMPLEMENTED(vector<aggregators::series*>, search_series, ());
    THROW_UNIMPLEMENTED(void, display_series, (aggregators::series& series));
    THROW_UNIMPLEMENTED(void, download_episodes, (aggregators::download_selection& download_selection));
};
