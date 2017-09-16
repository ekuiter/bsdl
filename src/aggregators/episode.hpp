#pragma once

#include <iostream>
#include "video_file.hpp"
#include <json.hpp>

using namespace std;

namespace aggregators {
    class episode {
    public:
        typedef unordered_map<providers::provider*, aggregators::video_file*> video_file_map;
        
    protected:
        mutable video_file_map video_files;
        string series_title;
        int season_number, number;
        mutable int skipped_video_files;
        mutable bool error_logged;
        http::request request;
        mutable bool loaded;
        mutable string id;
        static int max_id_width;
        
        virtual void load(const http::response& response) const = 0;
        
        episode(const string& _series_title, const int _season_number, const int _number, const video_file_map& _video_files,
                    const http::request& _request):
                    video_files(_video_files), series_title(_series_title), season_number(_season_number), number(_number),
                    skipped_video_files(0), error_logged(false), request(_request), loaded(false) {}
        
    public:
        class download;
        class file;
            
        string get_series_title() const noexcept {
            return series_title;
        }

        int get_season_number() const noexcept {
            return season_number;
        }

        int get_number() const noexcept {
            return number;
        }
            
        const video_file_map& get_video_files() const {
            load();
            return video_files;
        }
        
        string get_description() const {
            return get_id() == "" ? series_title : get_id();
        }

        void reset_skipped_video_files() const {
            skipped_video_files = 0;
        }

        const video_file* next_preferred_video_file() const {
            load();
            int skip = skipped_video_files++;
            const vector<providers::provider*>& preferred_providers = providers::provider::get_preferred_providers();
            for (auto& preferred_provider : preferred_providers)
                if (video_files.find(preferred_provider) != video_files.end() && skip-- == 0)
                    return video_files.at(preferred_provider);
            
            for (auto pair : video_files)
                if (skip-- == 0)
                    return pair.second;

            skipped_video_files--;
            if (!error_logged) {
                cerr << "there are no video files for " << get_description() << endl;
                error_logged = true;
            }
            return new video_file::unavailable();
        }

        virtual string get_id() const;        
        void load() const;
        virtual ostream& print(ostream& stream) const = 0;
        virtual unique_ptr<file> get_file() const = 0;
        unique_ptr<download> get_download(function<void (download&)> _refresh_callback = [](download&) {}) const;
        virtual nlohmann::json get_json() const;
        
        friend ostream& operator<<(ostream& stream, const episode& episode) {
            return episode.print(stream);
        }
    };
}
