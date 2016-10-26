#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <boost/range/adaptor/map.hpp>
#include "episode.hpp"
#include "exception.hpp"
#include "../http/client.hpp"

using namespace std;

namespace curses {
    class window;
}

namespace bs {
    class season_view;

    class season {
        typedef map<int, episode> episode_map;
        typedef boost::iterators::transform_iterator<
            boost::range_detail::select_second_mutable<episode_map>, episode_map::iterator> episode_iterator;
        typedef boost::iterators::transform_iterator<
            boost::range_detail::select_second_const<episode_map>, episode_map::const_iterator> episode_const_iterator;

        string series_title;
        int number;
        http::request request;
        mutable bool loaded;
        mutable episode_map episodes;
        shared_ptr<season_view> view;

        void load(const http::response& response) const;

        void add_episode(const episode& episode) const {
            episodes.insert({episode.get_number(), episode});
        }

    public:
        season(const string& _series_title, const int _number, const http::request& _request):
                series_title(_series_title), number(_number), request(_request), loaded(false) { }

        season(const string& _series_title, const int _number, const http::response& response):
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

        const episode& operator[](int number) const {
            return (*const_cast<season*>(this))[number];
        }

        episode& operator[](int number) {
            load();
            try {
                return episodes.at(number);
            } catch (out_of_range) {
                throw exception("episode #" + to_string(number) + " not found");
            }
        }

        episode_const_iterator begin() const {
            load();
            return boost::adaptors::values(const_cast<const episode_map&>(episodes)).begin();
        }

        episode_const_iterator end() const {
            load();
            return boost::adaptors::values(const_cast<const episode_map&>(episodes)).end();
        }

        episode_iterator begin() {
            load();
            return boost::adaptors::values(episodes).begin();
        }

        episode_iterator end() {
            load();
            return boost::adaptors::values(episodes).end();
        }

        void load() const;
        void create_view(curses::window& window);
        void destroy_view();
    };

    ostream& operator<<(ostream& stream, const season& season);
}
