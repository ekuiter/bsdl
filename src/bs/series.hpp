#pragma once

#include <iostream>
#include <map>
#include <boost/range/adaptor/map.hpp>
#include "../http/client.hpp"
#include "season.hpp"
#include "exception.hpp"
#include "../util/platform.hpp"
#include "../util/with_map_range.hpp"

using namespace std;

namespace bs {
    class series_base {
    protected:
        typedef map<int, season*> map_type;

    private:
        string title;
        http::request request;
        mutable bool loaded;
        mutable map_type seasons;

        void load(const http::response& response) const;
        
        void add_season(season* season) const {
            seasons.insert({season->get_number(), season});
        }
        
    protected:
        map_type& get_map() const {
            return seasons;
        }

    public:
        series_base(const string& _title, const http::request& _request): title(_title), request(_request), loaded(false) {
            title = util::platform::encode(title);
        }

        series_base(const string& _title, const http::response& response): title(_title) {
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

        const season* operator[](int number) const {
            return (*const_cast<series_base*>(this))[number];
        }

        season* operator[](int number) {
            load();
            try {
                return seasons.at(number);
            } catch (out_of_range) {
                throw exception("season #" + to_string(number) + " not found");
            }
        }

        void load() const;
    };
    
    ostream& operator<<(ostream& stream, const series_base& series);
    
    typedef util::with_map_range<series_base> series;
}
