#pragma once

#include <iostream>

using namespace std;

namespace aggregators {
    class episode {
        virtual string get_series_title() const noexcept = 0;
        virtual int get_season_number() const noexcept = 0;
        virtual int get_number() const noexcept = 0;
        virtual const video_file_map& get_video_files() const = 0;
        virtual string get_id() const = 0;

        const video_file* get_preferred_video_file() const {
            const vector<providers::provider*>& preferred_providers = providers::provider::get_preferred_providers();
            for (auto& preferred_provider : preferred_providers)
                if (video_files.find(preferred_provider) != video_files.end())
                    return video_files.at(preferred_provider);

            if (video_files.size() == 0) {
                if (!error_logged) {
                    cerr << "there are no video files for " << get_id() << endl;
                    error_logged = true;
                }
                return &video_file::unavailable_video_file;
            }

            return video_files.begin()->second;
        }
    };
}
