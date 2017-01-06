#pragma once

#include <iostream>
#include <vector>
#include "download_selector.hpp"
#include "../util/with_range.hpp"

using namespace std;

namespace aggregators {
    class download_selection_base {
    protected:
        typedef vector<download_selector*> container_type;

    private:
        mutable container_type download_selectors;

    protected:
        container_type& get_container() const {
            return download_selectors;
        }

    public:
        download_selection_base() {}

        ~download_selection_base() {
            for (auto download_selector : download_selectors)
                delete download_selector;
        }

        void add(download_selector* _download_selector) {
            download_selectors.push_back(_download_selector);
        }

        void add(vector<episode*> episodes) {
            for (auto episode : episodes)
                download_selectors.push_back(new download_selector::episode(episode));
        }

        int size() const {
            return download_selectors.size();
        }

        void clear() {
            for (auto download_selector : download_selectors)
                delete download_selector;
            download_selectors.clear();
        }

        vector<episode*> get_episodes(series& _series) {
            download_selector::episode_set all_episodes;
            for (auto download_selector : download_selectors) {
                download_selector::episode_set episodes = download_selector->get_episodes(_series);
                all_episodes.insert(episodes.begin(), episodes.end());
            }
            return vector<episode*>(all_episodes.begin(), all_episodes.end());
        }

        friend ostream& operator<<(ostream& stream, download_selection_base& download_selection) {
            int i = 0;
            for (auto& download_selector : download_selection.download_selectors) {
                stream << *download_selector;
                if (++i < download_selection.size())
                    stream << ", ";
            }
            return stream;
        }
    };

    typedef util::with_range<download_selection_base> download_selection;
}
