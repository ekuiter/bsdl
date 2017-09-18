#include "app.hpp"
#include "util/download_menu.hpp"
#include "util/download_dialog.hpp"

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
