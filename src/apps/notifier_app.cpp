#include "notifier_app.hpp"

void notifier_app::initialize() {    
    if (settings.is_set("action"))
        throw runtime_error(settings["action"] + " is not available in notifier mode");
    
    aggregators::aggregator::set_preferred_aggregators(settings.get_preferred_aggregators());
    providers::provider::set_preferred_providers(settings.get_preferred_providers());
    aggregators::subtitle::set_preferred_subtitles(settings.get_preferred_subtitles());

    if (settings.is_set("notifier_command") && settings.is_set("notifier_values")) {
        string value = util::platform::exec(settings["notifier_command"]);
        boost::trim(value);
        vector<string> values = settings::build_vector<string>("notifier_values", nullptr);
        if (find(values.begin(), values.end(), value) == values.end())
            throw runtime_error(string("unexpected \"") + value + "\" from notifier command");
    }

    vector<aggregators::episode*> monitored_episodes;
    try {
        monitored_episodes = fetch_monitored_episodes(fetch_monitored_series(settings::get("monitor_file")), nullptr);
    } catch (const std::exception& e) {
        util::platform::notify("bsdl", typeid(e).name(), e.what());
        exit(EXIT_FAILURE);
    }
    current_series = nullptr;

    if (!monitored_episodes.empty()) {
        int i = 0;
        string subtitle = "", last_series_title = "";
        for (auto& episode : monitored_episodes) {
            string series_title = episode->get_series_title();
            if (series_title != last_series_title)
                subtitle += string(", ") + series_title;
            last_series_title = series_title;
        }
    
        util::platform::notify("bsdl", subtitle.substr(2), to_string(monitored_episodes.size()) + " episodes");
    }

    exit(EXIT_SUCCESS);
}
