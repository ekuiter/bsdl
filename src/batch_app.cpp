#include "batch_app.hpp"
#include "option.hpp"
#include "util/bsdl_uri.hpp"
#include "util/download_menu.hpp"
#include "util/download_dialog.hpp"

using namespace nlohmann;

struct invalid_json : public runtime_error {
    invalid_json(): runtime_error("input must be valid JSON episodes (see 'bsdl <series> -j')") {}
};

void batch_app::initialize() {    
    if (settings.is_set("action"))
        throw runtime_error(settings["action"] + " is not available in batch mode");
    
    aggregators::aggregator::set_preferred_aggregators(settings.get_preferred_aggregators());
    providers::provider::set_preferred_providers(settings.get_preferred_providers());
    aggregators::subtitle::set_preferred_subtitles(settings.get_preferred_subtitles());

    if (!settings.is_set("series_search"))
        throw runtime_error("no episodes given to download, run 'bsdl \"$(bsdl <series> -j)\" -b'");

    json _json = read_json(settings["series_search"]);
    
    window::plain loading_window(get_centered_bounds(-1, -1));
    stream _stream(loading_window, color::get_accent_color());
    _stream << stream::write_centered(string("Loading ") + to_string(_json.size()) + " JSON episodes ...") << stream::refresh();
                
    fetch_all_series(_json);
    vector<aggregators::episode*> all_episodes = fetch_all_episodes();
 
    window::framed episodes_window(get_centered_bounds());
    stream stream(episodes_window);
    stream << "Episodes queued for download:" << endl;
    unique_ptr<window::sub> menu_wrapper;
    unique_ptr<window::sub> buttons_wrapper;
    unique_ptr<menu::vertical<vector<aggregators::episode*>>> menu;
    unique_ptr<button_group> buttons;

    auto start_download = [this](const vector<aggregators::episode*>& episodes) {
        window::framed download_window(get_centered_bounds());
        util::download_dialog::run<aggregators::episode, aggregators::episode::download>(
            download_window, episodes);
        return false;
    };
    
    util::make_download_menu(episodes_window, menu_wrapper, buttons_wrapper, menu, buttons, all_episodes, start_download, {}, 1);
    dynamic_cast<menu::vertical<vector<aggregators::episode*>>::multi&>(*menu).toggle_all();
    input::instance().wait();
    exit(EXIT_SUCCESS);
}

json batch_app::read_json(const string& str) {
    json _json;
    try {
        _json = json::parse(settings["series_search"]);
        if (!_json.is_array() || _json.size() == 0)
            throw invalid_json();        
    } catch (std::exception e) {
        throw invalid_json();
    }
    return _json;
}

void batch_app::fetch_all_series(json _json) {
    try {
        for (auto episode_json : _json) {
            int season = episode_json["season"], episode = episode_json["episode"];
            string uri = episode_json["uri"];
            if (all_series.count(uri) == 0) {
                aggregators::series* series = &util::bsdl_uri(uri).fetch_series();
                series->load();
                all_series.insert({ uri, { series, {} } });
            }
            all_series[uri].second.add(new aggregators::download_selector::episode(season, episode));
            { // cache id (and bs id) because it depends on get_current_series()
                aggregators::series& series = *all_series[uri].first;
                set_current_series(series);
                { // equivalent to set_current_series for bs id
                    auto mergeable_series = dynamic_cast<aggregators::bs::mergeable_series*>(&series);
                    if (mergeable_series)
                        aggregators::bs::merge_transform::instance().fetch_source_series(mergeable_series);
                }
                auto episode_obj = (*series[season])[episode];
                episode_obj->get_id();
                auto bs_series = aggregators::bs::try_to_get_bs_series(series);
                if (bs_series) {
                    auto bs_episode = dynamic_cast<aggregators::bs::mergeable_episode*>(episode_obj)->get_bs_episode();
                    if (bs_episode)
                        bs_episode->get_id();
                }
            }
        }
    } catch (domain_error e) {
        throw invalid_json();
    }
    current_series = nullptr;
}

vector<aggregators::episode*> batch_app::fetch_all_episodes() {
    vector<aggregators::episode*> all_episodes;
    for (auto pair : all_series) {
        aggregators::series* series = pair.second.first;
        aggregators::download_selection& download_selection = pair.second.second;
        auto episodes = download_selection.get_episodes(*series);
        all_episodes.insert(all_episodes.end(), episodes.begin(), episodes.end());
    }
    return all_episodes;
}
