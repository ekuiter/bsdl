#pragma once

#include "plain_app.hpp"

class notifier_app : public plain_app {
    aggregators::series* current_series = nullptr;
    
    notifier_app() {}

public:
    static notifier_app& instance() {
        if (!_instance)
            _instance.reset(new notifier_app());
        return dynamic_cast<notifier_app&>(*_instance);
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
