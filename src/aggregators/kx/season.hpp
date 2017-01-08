#pragma once

#include <iostream>
#include <map>
#include <memory>
#include "exception.hpp"
#include "episode.hpp"
#include "../../http/client.hpp"
#include "../aggregator.hpp"

using namespace std;

namespace curses {
    class window;
}

namespace aggregators {
    namespace kx {
        class season : public aggregators::season {
        private:
            vector<int> episode_numbers;
            http::request mirrors_request;
            void load(const http::response& response) const override;

        public:
            using aggregators::season::season;

            season(const string& _series_title, const http::request& _mirrors_request,
                    const int _number, vector<int> _episode_numbers):
                    season(_series_title, _number, http::request::idle) {
                episode_numbers = _episode_numbers;
                mirrors_request = _mirrors_request;
            }

            virtual ostream& print(ostream& stream) const override;
        };
    }
}
