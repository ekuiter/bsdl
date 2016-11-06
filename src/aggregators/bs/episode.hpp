#pragma once

#include <iostream>
#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "../../http/client.hpp"
#include "../../providers/provider.hpp"
#include "../../util/platform.hpp"
#include "../episode.hpp"
#include "../episode_download.hpp"
#include "video_file.hpp"
#include "exception.hpp"

using namespace std;

namespace aggregators {
    namespace bs {
        class episode : public aggregators::episode {
        public:
            class download;
            class file;

        private:
            string title_de, title_en;

        public:
            episode(const string& _series_title, const int _season_number, const int _number, const string& _title_de,
                    const string& _title_en, const video_file_map& _video_files):
                    aggregators::episode(_series_title, _season_number, _number, _video_files),
                        title_de(_title_de), title_en(_title_en) {
                boost::trim(title_de);
                boost::trim(title_en);
                title_de = util::platform::encode(title_de);
                title_en = util::platform::encode(title_en);
            }

            const string& get_german_title() const noexcept {
                return title_de;
            }

            const string& get_english_title() const noexcept {
                return title_en;
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
            ostream& print(ostream& stream) const;
        };
    }
}
