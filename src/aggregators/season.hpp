#pragma once

#include <iostream>
#include "episode.hpp"
#include "../util/with_map_range.hpp"
#include "../curses/terminal.hpp"

using namespace std;

namespace aggregators {
    class season_view;

    class season_base {
    protected:
        typedef map<int, episode*> map_type;
        string series_title;
        int number;
        http::request request;
        mutable bool loaded;
        mutable map_type episodes;
        shared_ptr<season_view> view;

        virtual void load(const http::response& response) const = 0;

        void add_episode(episode* episode) const {
            episodes.insert({episode->get_number(), episode});
        }

        map_type& get_map() const {
            return episodes;
        }

    public:
        season_base(const string& _series_title, const int _number, const http::request& _request):
                series_title(_series_title), number(_number), request(_request), loaded(false) { }

        string get_series_title() const noexcept {
            return series_title;
        }

        int get_number() const noexcept {
            return number;
        }

        int episode_number() const {
            load();
            return episodes.size();
        }

        const episode* operator[](int number) const {
            return (*const_cast<season_base*>(this))[number];
        }

        episode* operator[](int number) {
            load();
            try {
                return episodes.at(number);
            } catch (out_of_range) {
                throw exception("episode #" + to_string(number) + " not found");
            }
        }

        void load() const;
        void create_view(curses::window& window);
        void destroy_view();
        virtual ostream& print(ostream& stream) const = 0;
        
        friend ostream& operator<<(ostream& stream, const season_base& season) {
            return season.print(stream);
        }
    };

    typedef util::with_map_range<season_base> season;
}
