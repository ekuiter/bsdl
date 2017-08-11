#include "season.hpp"
#include "pr.hpp"
#include "video_file.hpp"
#include "../../providers/provider.hpp"
#include "../../settings.hpp"
#include <unordered_map>
#include <Node.h>

namespace aggregators {
    namespace pr {
        void season::load(const http::response& response) const {
            unique_ptr<CDocument> document = response.parse();
            CSelection episode_nodes = document->find(settings::get("pr_episode_sel")).ASSERT_AT_LEAST(1);
            
            for (auto& episode_node : episode_nodes) {
                CSelection episode_row = CNode(episode_node).find("td").ASSERT_AT_LEAST(4);
                int episode_number = stoi(episode_row.nodeAt(0).text());
                string subtitle_name = subtitle.get_name();
                transform(subtitle_name.begin(), subtitle_name.end(), subtitle_name.begin(), ::tolower);
                CNode episode_cell = episode_row.find(settings::get("pr_episode_row_sel_1") +
                                                      subtitle_name + to_string(episode_number) + " " +
                                                      settings::get("pr_episode_row_sel_2")).ASSERT_NUM(1).nodeAt(0);
                add_episode(new episode(series_title, number, episode_number, pr::root().get_relative(episode_cell.attribute("href"))));
            }

            loaded = true;
        }

        ostream& season::print(ostream& stream) const {
            return stream << season_title;
        }
    }
}
