#pragma once

#include <iostream>
#include <map>
#include <memory>
#include "exception.hpp"
#include "episode.hpp"
#include "../../http/client.hpp"
#include "../aggregator.hpp"

using namespace std;

namespace aggregators {
    namespace mk {
        class season : public aggregators::season {
        private:
            map<int, http::request> episode_requests;
            void load(const http::response& response) const override;

        public:
            season(const string& _series_title, const int _number, const map<int, http::request>& _episode_requests):
                    aggregators::season(_series_title, _number, http::request::idle) {
                episode_requests = _episode_requests;
            }

            virtual ostream& print(ostream& stream) const override;
        };
    }
}
