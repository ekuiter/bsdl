#pragma once

#include <iostream>
#include <map>
#include <memory>
#include "episode.hpp"
#include "exception.hpp"
#include "../../http/client.hpp"
#include "../subtitle.hpp"

using namespace std;

namespace aggregators {
    namespace pr {
        class season : public aggregators::season {
            string season_title;
            aggregators::subtitle& subtitle;
            void load(const http::response& response) const override;

        public:
            season(const string& _series_title, const int _number, const http::request& _request, const string& _season_title, aggregators::subtitle& _subtitle):
                aggregators::season(_series_title, _number, _request), season_title(_season_title), subtitle(_subtitle) { }

            season(const string& _series_title, const int _number, const http::response& response, const string& _season_title, aggregators::subtitle& _subtitle):
                aggregators::season(_series_title, _number, http::request()), season_title(_season_title), subtitle(_subtitle) {
                load(response);
            }

            virtual ostream& print(ostream& stream) const override;
        };
    }
}
