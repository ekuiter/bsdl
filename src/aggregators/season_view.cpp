#include "season_view.hpp"
#include "season.hpp"
#include "download_selection.hpp"
#include "../app.hpp"
#include "../util/bsdl_uri.hpp"
#include "../util/download_menu.hpp"
#include <sstream>

using namespace curses;

namespace aggregators {    
    season_view::season_view(season& _season, window& window) {
        stringstream s;
        s << "Loading " << _season << " ...";
        stream _stream(window, color::get_accent_color());
        _stream << stream::write_centered(s.str()) << stream::refresh();
        _season.load();
        _stream << stream::clear();

        auto start_download = [](const vector<episode*>& episodes) {
            aggregators::download_selection _download_selection;
            _download_selection.add(episodes);
            app::instance().download_episodes(_download_selection);
            return true;
        };

        util::make_download_menu(window, menu_wrapper, buttons_wrapper, menu, buttons, _season, start_download, {
                util::make_button_descriptor("Copy URI", 'u', []() {
                        util::platform::copy(util::bsdl_uri(*app::instance().get_current_series()).get_uri());
                        return true;
                    })
                });
    }
}
