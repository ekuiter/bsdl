#pragma once

#include <iostream>
#include <set>
#include "series.hpp"

using namespace std;

namespace bs {
    struct episode_pointer_compare {
        bool operator()(episode* lhs, episode* rhs) {
            if (lhs->get_season_number() == rhs->get_season_number())
                return lhs->get_number() < rhs->get_number();
            else
                return lhs->get_season_number() < rhs->get_season_number();
        }
    };

    class download_selector {
    public:
        typedef set<bs::episode*, episode_pointer_compare> episode_set;

        download_selector() {}
        virtual ~download_selector() {}

        virtual episode_set get_episodes(series& _series) const = 0;
        virtual operator string() const = 0;

        class series;
        class season;
        class episode;
        class latest_episode;
    };

    class download_selector::series : public download_selector {
    public:
        series() {}

        episode_set get_episodes(bs::series& _series) const override {
            episode_set episodes;
            for (auto& season : _series)
                for (auto episode : season)
                    episodes.insert(episode);
            return episodes;
        }

        operator string() const override {
            return "series";
        }
    };

    class download_selector::season : public download_selector {
        int season_number;

    public:
        season(int _season_number): season_number(_season_number) {}

        episode_set get_episodes(bs::series& _series) const override {
            episode_set episodes;
            for (auto episode : _series[season_number])
                episodes.insert(episode);
            return episodes;
        }

        operator string() const override {
            return string("season(") + to_string(season_number) + ")";
        }
    };

    class download_selector::episode : public download_selector {
        int season_number, number;

    public:
        episode(bs::episode* episode): season_number(episode->get_season_number()), number(episode->get_number()) {}
        episode(int _season_number, int _number): season_number(_season_number), number(_number) {}

        episode_set get_episodes(bs::series& _series) const override {
            return { _series[season_number][number] };
        }

        operator string() const override {
            return string("episode(") + to_string(season_number) + ", " + to_string(number) + ")";
        }
    };

    class download_selector::latest_episode : public download_selector {
    public:
        latest_episode() {}

        episode_set get_episodes(bs::series& _series) const override {
            bs::season* last_season;
            for (auto& season : _series)
                last_season = &season;
            bs::episode* last_episode;
            for (auto episode : *last_season)
                last_episode = episode;
            return { last_episode };
        }

        operator string() const override {
            return "latest_episode";
        }
    };

    ostream& operator<<(ostream& stream, download_selector& _download_selector);
}
