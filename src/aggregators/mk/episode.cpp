#include "episode.hpp"
#include "mk.hpp"
#include "../episode_download.hpp"
#include "../../curses/terminal.hpp"
#include "../../settings.hpp"
#include "episode_file.hpp"

namespace aggregators {
    namespace mk {
        void episode::load(const http::response& response) const {
            unique_ptr<CDocument> document = response.parse();
            CSelection video_file_nodes = document->find(settings::get("mk_video_file_sel"));
            CSelection script_nodes = document->find(settings::get("mk_video_file_script_sel"));
            
            for (auto& video_file_node : video_file_nodes) {
                string video_file_title = CNode(video_file_node).
                    find(settings::get("mk_video_file_title_sel")).assertNum(1).nodeAt(0).text().substr(3),
                    video_file_url = CNode(video_file_node).find("a").assertNum(2).nodeAt(0).attribute("href");
                providers::provider& provider = providers::provider::instance(video_file_title);
                video_files.insert({&provider, new video_file(provider, {mk::root().get_relative(video_file_url)})});
            }

            for (auto& script_node : script_nodes) {
                string script = CNode(script_node).text();
                vector<string> lines;
                boost::split(lines, script, boost::is_any_of("\n"));
                vector<http::request> requests;
                string video_file_title;
                for (auto& line : lines)
                    if (line.find("links[") == 0) {
                        smatch results;
                        assert(regex_search(line, results, regex(settings::get("mk_video_file_script_regex"))));
                        string video_file_url = results[1];
                        video_file_title = results[2];
                        requests.push_back(mk::root().get_relative(video_file_url));
                    }
                if (requests.size()) {
                    providers::provider& provider = providers::provider::instance(video_file_title);
                    video_files.insert({&provider, new video_file(provider, requests)});
                }
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
