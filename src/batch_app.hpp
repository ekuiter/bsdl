#pragma once

#include "curses_app.hpp"

class batch_app : public curses_app {
    vector<aggregators::series*> search_results;
    string series_search;
    aggregators::series* current_series = nullptr;
    unordered_map<string, pair<aggregators::series*, aggregators::download_selection>> all_series;
    
    batch_app() {}

public:
    static batch_app& instance() {
        if (!_instance)
            _instance.reset(new batch_app());
        return dynamic_cast<batch_app&>(*_instance);
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
    nlohmann::json read_json(const string& str);
    void fetch_all_series(nlohmann::json _json);
    vector<aggregators::episode*> fetch_all_episodes();

    THROW_UNIMPLEMENTED(vector<aggregators::series*>&, get_search_results, () const);
    THROW_UNIMPLEMENTED(string&, get_series_search, () const);
    THROW_UNIMPLEMENTED(aggregators::series&, choose_series, (vector<aggregators::series*>& search_results, const string& prompt, const string& action));
    THROW_UNIMPLEMENTED(vector<aggregators::series*>, search_series, ());
    THROW_UNIMPLEMENTED(void, display_series, (aggregators::series& series));
    THROW_UNIMPLEMENTED(void, download_episodes, (aggregators::download_selection& download_selection));
};
