#include "episode.hpp"
#include "kx.hpp"
#include "../episode_download.hpp"
#include "../../curses/terminal.hpp"
#include "../../settings.hpp"
#include "episode_file.hpp"

namespace aggregators {
    namespace kx {
        void episode::load(const http::response& response) const {
            unique_ptr<CDocument> document = response.parse();
            CSelection video_file_nodes = nullptr;
            try {
                video_file_nodes = document->find(settings::get("kx_video_file_sel")).assertAtLeast(1);
            } catch (runtime_error) {
                loaded = true;
                return;
            }

            for (auto& video_file_node : video_file_nodes) {
                string video_file_title = CNode(video_file_node).find(".Named").assertNum(1).nodeAt(0).text(),
                        video_file_parameters = CNode(video_file_node).attribute("rel");
                providers::provider& provider = providers::provider::instance(video_file_title);
                video_files.insert({&provider, new video_file(provider,
                        kx::root().get_relative(settings::get("kx_mirror_path") + video_file_parameters))});
            }

            loaded = true;
        }
        
        string episode::get_id() const {
            return file(*this).get_id();
        }
        
        ostream& episode::print(ostream& stream) const {
            if (bs_episode)
                return stream << *bs_episode;
            return stream << get_id() << curses::stream::move(max_id_width) << "Episode " << number;
        }
        
        unique_ptr<aggregators::episode::file> episode::get_file() const {
            return unique_ptr<aggregators::episode::file>(new episode::file(*this));
        }
    }
}
