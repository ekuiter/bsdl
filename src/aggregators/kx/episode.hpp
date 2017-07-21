#pragma once

#include <iostream>
#include <map>
#include <boost/algorithm/string.hpp>
#include "../../http/client.hpp"
#include "../../providers/provider.hpp"
#include "../../util/platform.hpp"
#include "../episode.hpp"
#include "../episode_download.hpp"
#include "exception.hpp"
#include "video_file.hpp"
#include "../bs/merge_transform.hpp"

using namespace std;

namespace aggregators {
    namespace kx {
        class episode : public bs::mergeable_episode {
        public:
            class file;

        protected:
            void load(const http::response& response) const override;

        public:
            episode(const string& _series_title, const int _season_number, const int _number, const http::request& mirrors_request):
                bs::mergeable_episode(_series_title, _season_number, _number, video_file_map(), mirrors_request) {}

            string get_id() const override;
            ostream& print(ostream& stream) const override;
            unique_ptr<aggregators::episode::file> get_file() const override;
        };
    }
}
