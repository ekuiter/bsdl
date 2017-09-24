#include "app.hpp"
#include "apps/main_app.hpp"
#include "util/download_menu.hpp"
#include "util/download_dialog.hpp"
#include "util/bsdl_uri.hpp"

unique_ptr<app> app::_instance = nullptr;

app& app::instance() {
    if (!_instance)
        _instance.reset(new main_app());
    return *_instance;
}

void app::cache_ids(aggregators::series& series, vector<aggregators::episode*> episodes) {
    // cache id (and bs id) because it depends on get_current_series()
    set_current_series(series);
    { // equivalent to set_current_series for bs id
        auto mergeable_series = dynamic_cast<aggregators::bs::mergeable_series*>(&series);
        if (mergeable_series)
            aggregators::bs::merge_transform::instance().fetch_source_series(mergeable_series);
    }
    for (auto episode : episodes) {
        episode->get_id();
        auto bs_series = aggregators::bs::try_to_get_bs_series(series);
        if (bs_series) {
            auto bs_episode = dynamic_cast<aggregators::bs::mergeable_episode*>(episode)->get_bs_episode();
            if (bs_episode)
                bs_episode->get_id();
        }
    }
}

void app::download_menu_then_exit(vector<aggregators::episode*> episodes) {
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
    
    util::make_download_menu(episodes_window, menu_wrapper, buttons_wrapper, menu, buttons, episodes, start_download, {}, 1);
    dynamic_cast<menu::vertical<vector<aggregators::episode*>>::multi&>(*menu).toggle_all();
    input::instance().wait();
    exit(EXIT_SUCCESS);
}

vector<string> app::fetch_monitored_series(const string& monitor_file_name) {
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

static void write_loading(stream* _stream, const string& title) {
    if (_stream)
        *_stream << stream::clear() << stream::write_centered(string("Loading ") + title + " ...") << stream::refresh();
}

vector<aggregators::episode*> app::fetch_monitored_episodes(vector<string> monitored_series, stream* _stream) {
    vector<aggregators::episode*> monitored_episodes;

    aggregators::download_selection& download_selection = settings::instance().get_download_selection();
    if (download_selection.size() == 0)
        download_selection.add(new aggregators::download_selector::new_episodes);
    
    for (auto uri : monitored_series) {
        write_loading(_stream, uri);
        aggregators::series* series = &util::bsdl_uri(uri).fetch_series();
        write_loading(_stream, series->get_title());
        series->load();
        
        auto episodes = download_selection.get_episodes(*series);
        cache_ids(*series, episodes);
        monitored_episodes.insert(monitored_episodes.end(), episodes.begin(), episodes.end());
    }
    
    return monitored_episodes;
}

void app::set_series_title(aggregators::series& series) {
    set_title(settings::instance().is_set("override_title") ?
              series.set_title(settings::instance()["override_title"]) :
              series.get_title());
}
