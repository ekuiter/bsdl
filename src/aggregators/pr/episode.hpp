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
    namespace pr {
        class episode : public aggregators::episode {
        public:
            class file;

        private:
            void load(const http::response& response) const override;

        public:
            episode(const string& _series_title, const int _season_number, const int _number, const http::request& _request):
                aggregators::episode(_series_title, _season_number, _number, video_file_map(), _request) {}

            string get_id() const override;
            ostream& print(ostream& stream) const override;
            unique_ptr<aggregators::episode::file> get_file() const override;
        };
    }
}
