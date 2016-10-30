#pragma once

#include <iostream>
#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "../../http/client.hpp"
#include "../../providers/provider.hpp"
#include "../../util/platform.hpp"
#include "video_file.hpp"
#include "exception.hpp"

using namespace std;

namespace aggregators {
    namespace bs {
        class episode {
        public:
            typedef unordered_map<providers::provider*, aggregators::video_file*> video_file_map;
            class download;
            class file;

        private:
            string series_title;
            int season_number, number;
            string title_de, title_en;
            video_file_map video_files;
            mutable bool error_logged;

        public:
            episode(const string& _series_title, const int _season_number, const int _number, const string& _title_de,
                    const string& _title_en, const video_file_map& _video_files):
                    series_title(_series_title), season_number(_season_number), number(_number), title_de(_title_de),
                    title_en(_title_en), video_files(_video_files), error_logged(false) {
                boost::trim(title_de);
                boost::trim(title_en);
                title_de = util::platform::encode(title_de);
                title_en = util::platform::encode(title_en);
            }

            string get_series_title() const noexcept {
                return series_title;
            }

            int get_season_number() const noexcept {
                return season_number;
            }

            int get_number() const noexcept {
                return number;
            }

            const string& get_german_title() const noexcept {
                return title_de;
            }

            const string& get_english_title() const noexcept {
                return title_en;
            }

            const video_file_map& get_video_files() const {
                return video_files;
            }

            const aggregators::video_file* get_preferred_video_file() const {
                const vector<providers::provider*>& preferred_providers = providers::provider::get_preferred_providers();
                for (auto& preferred_provider : preferred_providers)
                    if (video_files.find(preferred_provider) != video_files.end())
                        return video_files.at(preferred_provider);

                if (video_files.size() == 0) {
                    if (!error_logged) {
                        cerr << "there are no video files for " << get_id() << endl;
                        error_logged = true;
                    }
                    return new video_file::unavailable();
                }

                return video_files.begin()->second;
            }

            string get_id() const;
            download get_download(function<void (episode::download&)> _refresh_callback = [](episode::download&) {}) const;
        };

        ostream& operator<<(ostream& stream, const episode& episode);
    }
}
