#include "batch_app.hpp"
#include "option.hpp"
#include "util/bsdl_uri.hpp"

using namespace nlohmann;

struct invalid_json : public runtime_error {
    invalid_json(): runtime_error("input must be valid JSON episodes (see 'bsdl <series> -j')") {}
};

void batch_app::initialize() {
    if (settings.is_set("log_file")) {
        log_file.open(settings["log_file"]);
        terminal.get_stream(cout).set_log_file(&log_file);
        terminal.get_stream(cerr).set_log_file(&log_file);
        terminal.get_stream(clog).set_log_file(&log_file);
    }

    http::client::instance().set_timeout(stoi(settings::get("timeout")));
    set_title("bsdl");
    clog << "bsdl initialized in batch mode." << endl
         << endl << "Settings:" << endl << settings << endl;
    
    if (settings.is_set("action"))
        throw runtime_error(settings["action"] + " is not available in batch mode");
    
    aggregators::aggregator::set_preferred_aggregators(settings.get_preferred_aggregators());
    providers::provider::set_preferred_providers(settings.get_preferred_providers());
    aggregators::subtitle::set_preferred_subtitles(settings.get_preferred_subtitles());

    if (!settings.is_set("series_search"))
        throw runtime_error("no episodes given to download, run 'bsdl \"$(bsdl <series> -j)\" -b'");
    fetch_all_series(read_json(settings["series_search"]));
    vector<aggregators::episode*> all_episodes = fetch_all_episodes();

    cout << "Episodes queued to download:" << endl;
    for (auto episode : all_episodes)
        cout << *episode << endl;
    terminal.get_input().wait_any_key();
    
    exit(EXIT_SUCCESS);
}

json batch_app::read_json(const string& str) {
    json _json;
    try {
        _json = json::parse(settings["series_search"]);
        if (!_json.is_array())
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
            { // cache id because it depends on get_current_series()
                aggregators::series& series = *all_series[uri].first;
                set_current_series(series);
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

void batch_app::set_title(const string& title, bool set_notice, string notice) {
    if (!set_notice)
        clog << endl << title << endl << stream::write("-", title.length()) << endl;
}
