#include "season.hpp"
#include "bs.hpp"
#include "video_file.hpp"
#include "season_view.hpp"
#include "../../providers/provider.hpp"
#include "../../settings.hpp"
#include "../../curses/terminal.hpp"
#include <unordered_map>
#include <Node.h>

namespace aggregators {
    namespace bs {
        void season_base::load() const {
            if (!loaded) {
                cout << "Loading " << curses::color::get_accent_color() <<
                        *this << curses::color::previous << "." << endl;
                load(request());
            }
        }

        void season_base::load(const http::response& response) const {
            unique_ptr<CDocument> document = response.parse();
            CSelection episode_nodes = document->find(settings::get("episode_sel")).assertAtLeast(1);

            for (auto& episode_node : episode_nodes) {
                CSelection episode_row = CNode(episode_node).find("td").assertNum(3);
                int episode_number = stoi(episode_row.nodeAt(0).text());
                string episode_title_de = episode_row.nodeAt(1).find("a strong").nodeNum() == 1 ?
                                          episode_row.nodeAt(1).find("a strong").nodeAt(0).text() : "";
                string episode_title_en = episode_row.nodeAt(1).find("a span").nodeNum() == 1 ?
                                          episode_row.nodeAt(1).find("a span").nodeAt(0).text() : "";

                CSelection video_file_nodes = episode_row.nodeAt(2).find("a");
                episode::video_file_map video_files;
                for (auto& video_file_node : video_file_nodes) {
                    string video_file_title = CNode(video_file_node).text(),
                            video_file_url = CNode(video_file_node).attribute("href");
                    providers::provider& provider = providers::provider::instance(video_file_title);
                    video_files.insert({&provider, new video_file(provider, bs::root().get_relative(video_file_url))});
                }

                add_episode(new episode(series_title, number, episode_number, episode_title_de, episode_title_en, video_files));
            }

            loaded = true;
        }

        void season_base::create_view(curses::window& window) {
            if (view)
                throw exception("season view already exists");
            view.reset(new season_view(static_cast<season&>(*this), window));
        }

        void season_base::destroy_view() {
            view.reset();
        }

        ostream& operator<<(ostream& stream, const season_base& season) {
            if (season.get_number() == 0)
                return stream << "Movies";
            return stream << "Season #" << season.get_number();
        }
    }
}
