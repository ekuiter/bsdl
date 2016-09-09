#pragma once

#include <iostream>
#include <vector>
#include "download_selector.hpp"

using namespace std;

namespace bs {
    class download_selection {
        vector<download_selector*> download_selectors;

    public:
        download_selection() {}

        ~download_selection() {
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

        vector<download_selector*>::iterator begin() {
            return download_selectors.begin();
        }

        vector<download_selector*>::iterator end() {
            return download_selectors.end();
        }
    };

    ostream& operator<<(ostream& stream, download_selection& download_selection);
}
