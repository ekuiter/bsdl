#include "series.hpp"
#include "movie_season.hpp"
#include "episode_file.hpp"
#include "mk.hpp"
#include "../../curses/terminal.hpp"
#include "../../settings.hpp"
#include <Node.h>
#include <boost/algorithm/string.hpp>

namespace aggregators {
    namespace mk {
        void series::load(const http::response& response) const {
            unique_ptr<CDocument> document = response.parse();
            CSelection season_nodes = document->find(settings::get("mk_season_sel") + " option");
            
            bs::merge_transform::instance().fetch_source_series();
            
            if (season_nodes.nodeNum() == 0)
                add_season(new movie_season(title, response));
            else
                for (auto& season_node : season_nodes) {
                    int season_number = stoi(CNode(season_node).attribute("value"));
                    CSelection episode_nodes = document->find(settings::get("mk_episode_sel") + to_string(season_number) + " option");
                    map<int, http::request> episode_requests;
                    for (auto& episode_node : episode_nodes) {
                        string value = CNode(episode_node).text();
                        if (value != "") {
                            vector<string> parts;
                            boost::split(parts, value, boost::is_any_of(" "));
                            episode_requests[stoi(parts[1])] = mk::root().get_relative(CNode(episode_node).attribute("value"));
                        }
                    }
                    add_season(new season(title, season_number, episode_requests));
                }
            
            loaded = true;
        }

        ostream& series::print(ostream& stream) const {
            curses::window* window = curses::terminal::instance().get_stream(stream).get_window();
            int width = window ? window->get_bounds().width : COLS;
            
            if (aggregator::get_preferred_aggregators().size() > 1)
                stream << get_aggregator() << curses::stream::move(max_aggregator_width);
            return stream << curses::stream::write_truncated(language, 4) << " " <<
                    util::platform::strip_chars(get_title());
        }
        
        unique_ptr<aggregators::episode::file> series::get_file(const string& old_file_name,
            const string& pattern_str) {
            return unique_ptr<aggregators::episode::file>(new episode::file(*this, old_file_name, pattern_str));
        }
    }
}
