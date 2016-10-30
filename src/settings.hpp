#pragma once

#include <iostream>
#include <set>
#include "aggregators/bs/bs.hpp"
#include "util/with_range.hpp"

using namespace std;

class settings_base {
protected:
    typedef map<string, string> container_type;
       
    settings_base(): allowed_settings({"series_search", "output_files_directory", "rename_files_directory",
          "rename_files_pattern", "show_info", "log_file", "config_file", "root_url", "search_path", "series_sel",
          "season_sel", "episode_sel", "video_file_sel", "movies_text", "providers", "provider_v",
          "provider_v_file_format", "provider_s", "provider_s_file_format"}) {
        for (auto& setting : allowed_settings)
            settings_map[setting];
    }
          
private:
    aggregators::bs::download_selection _download_selection;
    vector<providers::provider*> preferred_providers;
    mutable container_type settings_map;
    set<string> allowed_settings;
    
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

    aggregators::bs::download_selection& get_download_selection() {
        return _download_selection;
    }

    vector<providers::provider*> get_preferred_providers() {
        return preferred_providers;
    }

    void read(const vector<string>& args);
};

typedef util::with_range<settings_base> settings;

ostream& operator<<(ostream& stream, settings& _settings);