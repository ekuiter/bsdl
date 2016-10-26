#pragma once

#include <iostream>
#include <map>
#include <boost/range/adaptor/map.hpp>
#include "../http/client.hpp"
#include "season.hpp"
#include "exception.hpp"
#include "../util/platform.hpp"

using namespace std;

namespace bs {
    class series {
        typedef map<int, season> season_map;

        string title;
        http::request request;
        mutable bool loaded;
        mutable season_map seasons;

        void load(const http::response& response) const;

        void add_season(const season& season) const {
            seasons.insert({season.get_number(), season});
        }

    public:
        series(const string& _title, const http::request& _request): title(_title), request(_request), loaded(false) {
            title = util::platform::encode(title);
        }

        series(const string& _title, const http::response& response): title(_title) {
            title = util::platform::encode(title);
            load(response);
        }

        const string& get_title() const noexcept {
            return title;
        }

        int season_number() const {
            load();
            return seasons.size();
        }

        const season& operator[](int number) const {
            return (*const_cast<series*>(this))[number];
        }

        season& operator[](int number) {
            load();
            try {
                return seasons.at(number);
            } catch (out_of_range) {
                throw exception("season #" + to_string(number) + " not found");
            }
        }

        decltype(boost::adaptors::values(const_cast<const season_map&>(seasons)).begin()) begin() const {
            load();
            return boost::adaptors::values(const_cast<const season_map&>(seasons)).begin();
        }

        decltype(boost::adaptors::values(const_cast<const season_map&>(seasons)).end()) end() const {
            load();
            return boost::adaptors::values(const_cast<const season_map&>(seasons)).end();
        }

        decltype(boost::adaptors::values(seasons).begin()) begin() {
            load();
            return boost::adaptors::values(seasons).begin();
        }

        decltype(boost::adaptors::values(seasons).end()) end() {
            load();
            return boost::adaptors::values(seasons).end();
        }

        void load() const;
    };

    ostream& operator<<(ostream& stream, const series& series);
}
