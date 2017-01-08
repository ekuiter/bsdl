#pragma once

#include <iostream>
#include <set>
#include "aggregators/aggregator.hpp"
#include "util/with_range.hpp"

using namespace std;

class settings_base {
protected:
    typedef map<string, string> container_type;
       
    settings_base() {
        for (int i = 1; i <= providers::provider::count; i++) {
            allowed_settings.insert(string("provider_") + to_string(i));
            allowed_settings.insert(string("provider_") + to_string(i) + "_file_format");
        }
        for (auto& setting : allowed_settings)
            settings_map[setting];
    }
          
private:
    aggregators::download_selection _download_selection;
    vector<aggregators::aggregator*> preferred_aggregators;
    vector<providers::provider*> preferred_providers;
    mutable container_type settings_map;
    static set<string> allowed_settings;
    
protected:
    container_type& get_container() const {
        return settings_map;
    }

public:
    static util::with_range<settings_base>& instance() {
        static util::with_range<settings_base> instance;
        return instance;
    }

    static string& get(const string& key) {
        if (!instance().is_set(key))
            throw runtime_error(string("setting \"") + key + "\" not set, check config file");
        return instance()[key];
    }

    string& operator[](const string& key) {
        if (allowed_settings.find(key) == allowed_settings.end())
            throw runtime_error(string("setting \"") + key + "\" not allowed, check config file");
        return settings_map[key];
    }

    bool is_set(const string& key) {
        return settings_map[key] != "";
    }

    aggregators::download_selection& get_download_selection() {
        return _download_selection;
    }
    
    vector<aggregators::aggregator*> get_preferred_aggregators() {
        return preferred_aggregators;
    }

    vector<providers::provider*> get_preferred_providers() {
        return preferred_providers;
    }

    void read(const vector<string>& args);
    
    friend ostream& operator<<(ostream& stream, settings_base& settings);
};

typedef util::with_range<settings_base> settings;
