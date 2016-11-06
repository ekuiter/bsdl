#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <boost/range/adaptor/map.hpp>
#include "episode.hpp"
#include "exception.hpp"
#include "../../http/client.hpp"
#include "../../util/with_map_range.hpp"

using namespace std;

namespace curses {
    class window;
}

namespace aggregators {
    namespace bs {
        class season : public aggregators::season {
        private:
            void load(const http::response& response) const override;

        public:
            using aggregators::season::season;

            season(const string& _series_title, const int _number, const http::response& response):
                    season(_series_title, _number, http::request()) {
                load(response);
            }

            virtual ostream& print(ostream& stream) const override;
        };
    }
}
