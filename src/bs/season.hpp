#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <boost/range/adaptor/map.hpp>
#include "episode.hpp"
#include "exception.hpp"
#include "../http/client.hpp"
#include "../util/with_map_range.hpp"

using namespace std;

namespace curses {
    class window;
}

namespace bs {
    class season_view;

    class season_base {
    protected:
        typedef map<int, episode*> map_type;
        
    private:
        string series_title;
        int number;
        http::request request;
        mutable bool loaded;
        mutable map_type episodes;
        shared_ptr<season_view> view;

        void load(const http::response& response) const;

        void add_episode(episode* episode) const {
            episodes.insert({episode->get_number(), episode});
        }
        
    protected:
        map_type& get_map() const {
            return episodes;
        }

    public:
        season_base(const string& _series_title, const int _number, const http::request& _request):
                series_title(_series_title), number(_number), request(_request), loaded(false) { }

        season_base(const string& _series_title, const int _number, const http::response& response):
                series_title(_series_title), number(_number) {
            load(response);
        }

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
    };

    ostream& operator<<(ostream& stream, const season_base& season);
    
    typedef util::with_map_range<season_base> season;
}
