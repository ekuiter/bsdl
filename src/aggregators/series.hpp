#pragma once

#include <iostream>
#include "season.hpp"
#include "../util/with_map_range.hpp"
#include "../util/platform.hpp"

using namespace std;

namespace aggregators {
    class series_base {
    protected:
        typedef map<int, season*> map_type;
        string title;
        http::request request;
        mutable bool loaded;
        mutable map_type seasons;

        virtual void load(const http::response& response) const = 0;

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
        virtual ostream& print(ostream& stream) const = 0;
        virtual unique_ptr<episode::file> get_file(const string& old_file_name, const string& pattern_str) = 0;
        
        friend ostream& operator<<(ostream& stream, const series_base& series) {
            return series.print(stream);
        }
    };

    typedef util::with_map_range<series_base> series;
}
