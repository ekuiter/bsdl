#pragma once

#include <iostream>
#include <map>
#include <memory>
#include "exception.hpp"
#include "episode.hpp"
#include "season.hpp"
#include "../../http/client.hpp"
#include "../aggregator.hpp"

using namespace std;

namespace aggregators {
    namespace mk {
        class movie_season : public season {
        private:
            void load(const http::response& response) const override;

        public:
            movie_season(const string& _series_title, const http::response& response):
                    season(_series_title, 1, {}) {
                load(response);
            }

            virtual ostream& print(ostream& stream) const override;
        };
    }
}
