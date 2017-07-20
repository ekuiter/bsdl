#include "movie_episode.hpp"
#include "mk.hpp"
#include "../episode_download.hpp"
#include "../../curses/terminal.hpp"
#include "../../settings.hpp"
#include "episode_file.hpp"

namespace aggregators {
    namespace mk {
        void movie_episode::load(const http::response& response) const {
            unique_ptr<CDocument> document = response.parse();
            CSelection video_file_nodes = document->find(settings::get("mk_video_file_sel"));
            CSelection script_nodes = document->find(settings::get("mk_video_file_script_sel"));
            
            for (auto& video_file_node : video_file_nodes) {
                CNode link_node = CNode(video_file_node).find("a").assertNum(2).nodeAt(0);
                if (link_node.childNum() != 3)
                    throw std::runtime_error(std::string("node has ") + std::to_string(link_node.childNum()) + " children instead of 3");
                string video_file_title = link_node.childAt(2).text().substr(3),
                    video_file_url = link_node.attribute("href");
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
                        assert(regex_search(line, results, regex(settings::get("mk_video_file_movie_regex"))));
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
        
        string movie_episode::get_id() const {
            return "";
        }
        
        ostream& movie_episode::print(ostream& stream) const {
            return stream << series_title;
        }
    }
}
