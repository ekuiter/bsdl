#pragma once

#include <iostream>
#include <map>
#include <boost/algorithm/string.hpp>
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
            class file;

        private:
            string title_de, title_en;
            void load(const http::response& response) const override;

        public:
            episode(const string& _series_title, const int _season_number, const int _number, const string& _title_de,
                    const string& _title_en, const video_file_map& _video_files):
                    aggregators::episode(_series_title, _season_number, _number, _video_files, http::request::idle),
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

            string get_id() const override;
            ostream& print(ostream& stream) const override;
            unique_ptr<aggregators::episode::file> get_file() const override;
        };
    }
}
