#include "monitor_app.hpp"
#include "option.hpp"
#include "util/bsdl_uri.hpp"

void monitor_app::initialize() {    
    if (settings.is_set("action"))
        throw runtime_error(settings["action"] + " is not available in monitor mode");
    
    aggregators::aggregator::set_preferred_aggregators(settings.get_preferred_aggregators());
    providers::provider::set_preferred_providers(settings.get_preferred_providers());
    aggregators::subtitle::set_preferred_subtitles(settings.get_preferred_subtitles());

    if (settings.is_set("series_search"))
        throw runtime_error("search is not available in monitor mode");

    vector<string> monitored_series = fetch_monitored_series(settings::get("monitor_file"));
    download_menu_then_exit(fetch_monitored_episodes(monitored_series));
}

vector<string> monitor_app::fetch_monitored_series(const string& monitor_file_name) {
    if (!boost::filesystem::exists(monitor_file_name))
        throw runtime_error(string("monitor file \"") + monitor_file_name + "\" does not exist");
    std::ifstream monitor_file;
    monitor_file.open(monitor_file_name);
    
    vector<string> monitored_series;
    regex pattern("(bsdl://.*?)(?:[\\s\\]]|$)"); // matches URIs anywhere in line
    string line;
    while (getline(monitor_file, line)) {
        smatch results;
        if (!regex_search(line, results, pattern))
            continue;
        monitored_series.push_back(results[1]);
    }
    return monitored_series;
}

vector<aggregators::episode*> monitor_app::fetch_monitored_episodes(vector<string> monitored_series) {
    vector<aggregators::episode*> monitored_episodes;
    if (monitored_series.empty())
        throw runtime_error("no monitored series found, check monitor file");

    aggregators::download_selection& download_selection = settings.get_download_selection();
    if (download_selection.size() == 0)
        download_selection.add(new aggregators::download_selector::new_episodes);
    
    for (auto uri : monitored_series) {
        window::plain loading_window(get_centered_bounds(-1, -1));
        stream _stream(loading_window, color::get_accent_color());
        _stream << stream::write_centered(string("Loading ") + uri + " ...") << stream::refresh();
        
        aggregators::series* series = &util::bsdl_uri(uri).fetch_series();
        _stream << stream::clear() << stream::write_centered(string("Loading ") + series->get_title() + " ...") << stream::refresh();
        series->load();
        
        auto episodes = download_selection.get_episodes(*series);
        cache_ids(*series, episodes);
        monitored_episodes.insert(monitored_episodes.end(), episodes.begin(), episodes.end());
    }
    
    current_series = nullptr;
    return monitored_episodes;
}
