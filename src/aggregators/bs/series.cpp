#include "series.hpp"
#include "bs.hpp"
#include "../../curses/terminal.hpp"
#include "../../settings.hpp"
#include <Node.h>

namespace aggregators {
    namespace bs {
        void series::load(const http::response& response) const {
            unique_ptr<CDocument> document = response.parse();
            CSelection season_nodes = document->find(settings::get("bs_season_sel")).ASSERT_AT_LEAST(1);

            for (auto& season_node : season_nodes) {
                string season_text = CNode(season_node).text();
                int season_number = season_text == settings::get("bs_movies_text") ? 0 : stoi(season_text);
                string season_url = CNode(season_node).attribute("href");
                http::request season_request(bs::root().get_relative(season_url));
                add_season(season_number == 1 ? new season(title, 1, response) : new season(title, season_number, season_request));
            }

            loaded = true;
        }

        ostream& series::print(ostream& stream) const {
            curses::window* window = curses::terminal::instance().get_stream(stream).get_window();
            int width = window ? window->get_bounds().width : COLS;
            
            if (aggregator::get_preferred_aggregators().size() > 1)
                stream << get_aggregator() << curses::stream::move(max_aggregator_width + 5);
            return stream << util::platform::strip_chars(get_title());
        }
        
        unique_ptr<aggregators::episode::file> series::get_file(const string& old_file_name,
            const string& pattern_str) {
            return unique_ptr<aggregators::episode::file>(new episode::file(*this, old_file_name, pattern_str));
        }
    }
}
