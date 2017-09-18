#pragma once

#include <iostream>
#include <set>
#include "series.hpp"

using namespace std;

namespace aggregators {
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
        typedef set<episode*, episode_pointer_compare> episode_set;

        download_selector() {}
        virtual ~download_selector() {}

        virtual episode_set get_episodes(aggregators::series& _series) const = 0;
        virtual operator string() const = 0;

        friend ostream& operator<<(ostream& stream, download_selector& _download_selector) {
            return stream << (string) _download_selector;
        }

        class series;
        class season;
        class episode;
        class latest_episode;
        class new_episodes;
    };

    class download_selector::series : public download_selector {
    public:
        series() {}

        episode_set get_episodes(aggregators::series& _series) const override;

        operator string() const override {
            return "series";
        }
    };

    class download_selector::season : public download_selector {
        int season_number;

    public:
        season(int _season_number): season_number(_season_number) {}

        episode_set get_episodes(aggregators::series& _series) const override;

        operator string() const override {
            return string("season(") + to_string(season_number) + ")";
        }
    };

    class download_selector::episode : public download_selector {
        int season_number, number;

    public:
        episode(aggregators::episode* episode): season_number(episode->get_season_number()), number(episode->get_number()) {}
        episode(int _season_number, int _number): season_number(_season_number), number(_number) {}

        episode_set get_episodes(aggregators::series& _series) const override;

        operator string() const override {
            return string("episode(") + to_string(season_number) + ", " + to_string(number) + ")";
        }
    };

    class download_selector::latest_episode : public download_selector {
    public:
        latest_episode() {}

        episode_set get_episodes(aggregators::series& _series) const override;

        operator string() const override {
            return "latest_episode";
        }
    };

    class download_selector::new_episodes : public download_selector {
    public:
        new_episodes() {}

        episode_set get_episodes(aggregators::series& _series) const override;

        operator string() const override {
            return "new_episodes";
        }
    };
}
