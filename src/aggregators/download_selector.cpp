#include "download_selector.hpp"
#include "episode_file.hpp"
#include "../settings.hpp"
#include "../util/string_similarity.hpp"

using namespace boost::filesystem;

namespace aggregators {
    download_selector::episode_set download_selector::series::get_episodes(aggregators::series& _series) const {
        episode_set episodes;
        for (auto season : _series)
            for (auto episode : *season)
                episodes.insert(episode);
        return episodes;
    }

    download_selector::episode_set download_selector::season::get_episodes(aggregators::series& _series) const {
        episode_set episodes;
        for (auto episode : *_series[season_number])
            episodes.insert(episode);
        return episodes;
    }

    download_selector::episode_set download_selector::episode::get_episodes(aggregators::series& _series) const {
        return { (*_series[season_number])[number] };
    }

    download_selector::episode_set download_selector::latest_episode::get_episodes(aggregators::series& _series) const {
        aggregators::season* last_season;
        for (auto& season : _series)
            last_season = season;
        aggregators::episode* last_episode;
        for (auto episode : *last_season)
            last_episode = episode;
        return { last_episode };
    }

    static download_selector::episode_set get_new_episodes(aggregators::series& _series, download_selector::episode_set& downloaded_episodes) {
        download_selector::episode_set new_episodes;
        aggregators::episode* start_episode = downloaded_episodes.empty() ? nullptr : *downloaded_episodes.begin();
        int start_season = start_episode ? start_episode->get_season_number() : 0;
        bool saw_start_episode = start_episode ? false : true;
        for (auto season : _series)
            if (season->get_number() >= start_season)
                for (auto episode : *season) {
                    if (saw_start_episode && downloaded_episodes.find(episode) == downloaded_episodes.end())
                        new_episodes.insert(episode);
                    if (episode == start_episode)
                        saw_start_episode = true;
                }
        return new_episodes;
    }

    static download_selector::episode_set get_downloaded_episodes(aggregators::series& _series, const string& directory) {
        directory_iterator end_it;
        download_selector::episode_set downloaded_episodes;
        string title = _series.get_title();
        if (!exists(directory))
            return downloaded_episodes;
        
        for (directory_iterator it(directory); it != end_it; it++)
            if (is_regular_file(it->status())) {
                try {
                    string old_file_name = it->path().filename().string();
                    if (util::get_string_similarity(old_file_name.substr(0, title.length()), title) > 0.8) {
                        unique_ptr<episode::file> file = _series.get_file(old_file_name, "default");
                        downloaded_episodes.insert(const_cast<aggregators::episode*>(file->get_episode()));
                    }
                } catch (aggregators::exception& e) {}
            }
        return downloaded_episodes;
    }

    download_selector::episode_set download_selector::new_episodes::get_episodes(aggregators::series& _series) const {
        download_selector::episode_set downloaded_episodes = get_downloaded_episodes(_series, settings::instance().output_files_directory(_series.get_title()));
        return get_new_episodes(_series, downloaded_episodes);
    }
}
