#include "episode.hpp"
#include "episode_download.hpp"
#include "episode_file.hpp"
#include "../app.hpp"
#include "../util/bsdl_uri.hpp"

namespace aggregators {
    int episode::max_id_width = 13;
    
    void episode::load() const {
        if (!loaded)
            load(request());
    }

    string episode::get_id() const {
        if (id == "") {
            string format_string = string("%0") + to_string(episode::file::get_season_digits()) + "d.%02d";
            id = boost::str(boost::format(format_string) % get_season_number() % get_number());
        }
        return id;
    }
    
    unique_ptr<episode::download> episode::get_download(episode::download::refresh_callback _refresh_callback) const {
        return unique_ptr<episode::download>(new episode::download(*this, _refresh_callback));
    }

    nlohmann::json episode::get_json() const {
        return {
            { "series", series_title },
            { "season", season_number },
            { "episode", number },
            { "uri", util::bsdl_uri(*app::instance().get_current_series()).get_uri() }
        };
    }
}
