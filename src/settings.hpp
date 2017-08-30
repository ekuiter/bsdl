#pragma once

#include <iostream>
#include <set>
#include <memory>
#include "aggregators/aggregator.hpp"
#include "aggregators/subtitle.hpp"
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
    vector<aggregators::subtitle*> preferred_subtitles;
    mutable container_type settings_map;
    static set<string> allowed_settings;
    typedef util::with_range<settings_base> settings_with_range;
    static unique_ptr<settings_with_range> _instance;
    void update_preferred_aggregators(bool clear_vector = false);
    void update_preferred_providers(bool clear_vector = false);
    void update_preferred_subtitles(bool clear_vector = false);
    
protected:
    container_type& get_container() const {
        return settings_map;
    }

public:
    static settings_with_range& instance() {
        return *_instance;
    }

    static settings_with_range& reset_instance() {
        _instance.reset(new settings_with_range());
        return *_instance;
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

    vector<aggregators::subtitle*> get_preferred_subtitles() {
        return preferred_subtitles;
    }

    template<typename T, typename U>
    void validate_usage(const vector<string>& args, int& i, int first_arg, T is_arg, U next_arg);
    void read_config_file();
    template<typename T, typename U>
    void process_args(const vector<string>& args, int& i, int first_arg, T is_arg, U next_arg);
    void read(const vector<string>& args);
    string resource_file(const string& filename);
    string default_config_file();
    string default_log_file();
    
    friend ostream& operator<<(ostream& stream, settings_base& settings);
    friend class option;
};

typedef util::with_range<settings_base> settings;
