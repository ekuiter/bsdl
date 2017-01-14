#pragma once

#include <iostream>
#include <map>
#include <boost/algorithm/string.hpp>
#include "../../http/client.hpp"
#include "../../providers/provider.hpp"
#include "../../util/platform.hpp"
#include "episode.hpp"
#include "../episode_download.hpp"
#include "exception.hpp"
#include "video_file.hpp"

using namespace std;

namespace aggregators {
    namespace kx {
        class movie_episode : public episode {
        public:
            movie_episode(const string& _series_title, const http::response& response):
                    episode(_series_title, 1, 1, http::request()) {
                load(response);
            }

            string get_id() const override;
            ostream& print(ostream& stream) const override;
        };
    }
}
