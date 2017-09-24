#include "monitor_app.hpp"

void monitor_app::initialize() {    
    if (settings.is_set("action"))
        throw runtime_error(settings["action"] + " is not available in monitor mode");
    
    aggregators::aggregator::set_preferred_aggregators(settings.get_preferred_aggregators());
    providers::provider::set_preferred_providers(settings.get_preferred_providers());
    aggregators::subtitle::set_preferred_subtitles(settings.get_preferred_subtitles());

    vector<aggregators::episode*> monitored_episodes;
    {
        window::plain loading_window(get_centered_bounds(-1, -1));
        stream _stream(loading_window, color::get_accent_color());
        monitored_episodes = fetch_monitored_episodes(fetch_monitored_series(settings::get("monitor_file")), &_stream);
    }
    current_series = nullptr;
    download_menu_then_exit(monitored_episodes);
}
