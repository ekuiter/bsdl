#pragma once

#include <iostream>
#include <set>
#include "bs/bs.hpp"

using namespace std;

class settings {
    settings(): allowed_settings({"series_search", "output_files_directory", "rename_files_directory",
          "rename_files_pattern", "show_info", "log_file", "config_file", "root_url", "search_path", "series_sel",
          "season_sel", "episode_sel", "video_file_sel", "movies_text", "providers", "provider_v",
          "provider_v_file_format", "provider_v_hash_sel", "provider_v_timestamp_sel", "provider_v_player_sel",
          "provider_s", "provider_s_file_format", "provider_s_op", "provider_s_player_sel", "provider_s_file_pattern"}) {
        for (auto& setting : allowed_settings)
            settings_map[setting];
    }

    bs::download_selection _download_selection;
    vector<providers::provider*> preferred_providers;
    map<string, string> settings_map;
    set<string> allowed_settings;

public:
    static settings& instance() {
        static settings instance;
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

    bs::download_selection& get_download_selection() {
        return _download_selection;
    }

    vector<providers::provider*> get_preferred_providers() {
        return preferred_providers;
    }

    map<string, string>::iterator begin() {
        return settings_map.begin();
    }

    map<string, string>::iterator end() {
        return settings_map.end();
    }

    void read(const vector<string>& args);
};

ostream& operator<<(ostream& stream, settings& _settings);