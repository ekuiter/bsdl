#include "episode.hpp"
#include "episode_download.hpp"
#include "../app.hpp"
#include "../util/bsdl_uri.hpp"

namespace aggregators {
    int episode::max_id_width = 13;
    
    void episode::load() const {
        if (!loaded)
            load(request());
    }
    
    unique_ptr<episode::download> episode::get_download(episode::download::refresh_callback _refresh_callback) const {
        return unique_ptr<episode::download>(new episode::download(*this, _refresh_callback));
    }

    nlohmann::json episode::get_json() const {
        return {
            { "series_title", series_title },
            { "season_number", season_number },
            { "number", number },
            { "bsdl_uri", util::bsdl_uri(*app::instance().get_current_series()).get_uri() }
        };
    }
}
