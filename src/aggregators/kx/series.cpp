#include "series.hpp"
#include "episode_file.hpp"
#include "kx.hpp"
#include "../../curses/terminal.hpp"
#include "../../settings.hpp"
#include <Node.h>
#include <boost/algorithm/string.hpp>

namespace aggregators {
    namespace kx {
        void series::load(const http::response& response) const {
            unique_ptr<CDocument> document = response.parse();
            CSelection season_nodes = document->find(settings::get("kx_season_sel") + " option");
            
            if (season_nodes.nodeNum() == 0)
                throw exception("movies are not implemented yet"); // @TODO
            else {
                string parameters = document->find(settings::get("kx_season_sel")).assertNum(1).nodeAt(0).attribute("rel");
                for (auto& season_node : season_nodes) {
                    int season_number = stoi(CNode(season_node).attribute("value"));
                    string episodes_string = CNode(season_node).attribute("rel");
                    vector<string> episode_strings;
                    vector<int> episode_numbers;
                    boost::split(episode_strings, episodes_string, boost::is_any_of(","));
                    transform(episode_strings.begin(), episode_strings.end(), back_inserter(episode_numbers),
                            [](const string& episode_string) { return stoi(episode_string); });
                    add_season(new season(title, kx::root().get_relative(settings::get("kx_mirror_by_episode_path") + parameters),
                            season_number, episode_numbers));
                }
            }
            
            loaded = true;
        }

        ostream& series::print(ostream& stream) const {
            return stream << util::platform::strip_chars(get_title());
        }
        
        unique_ptr<aggregators::episode::file> series::get_file(const string& old_file_name,
            const string& pattern_str) {
            return unique_ptr<aggregators::episode::file>(new episode::file(*this, old_file_name, pattern_str));
        }
    }
}
