#include "episode.hpp"
#include "../episode_download.hpp"
#include "episode_file.hpp"
#include "../../curses/terminal.hpp"
#include "../../settings.hpp"
#include <json.hpp>
#include <regex>
#include "../../util/platform.hpp"

using namespace nlohmann;

namespace aggregators {
    namespace pr {
        void episode::load(const http::response& response) const {           
            unique_ptr<CDocument> document = response.parse();
            if (document->find(settings::get("pr_captcha_sel")).nodeNum() > 0) {
                cerr << "You need to visit " << settings::get("aggregator_pr") << " in your browser and solve the captcha once." << endl;
                util::platform::sleep(chrono::seconds(3));
                cerr << "Visiting " << settings::get("aggregator_pr") << " ..." << endl;
                util::platform::browse(request.get_url());
                cerr << "When you have solved the captcha, try again." << endl;
                return;
            }
            
            CSelection script_nodes = document->find(settings::get("pr_video_file_sel")).ASSERT_AT_LEAST(1);
            string script = script_nodes.nodeAt(script_nodes.nodeNum() - 1).text();
            smatch results;
            assert(regex_search(script, results, regex(settings::get("pr_video_file_script_regex"))));
            script = results[1];
            
            for (auto& _video_file : json::parse(script)) {
                if (_video_file["type"] == "message") {
                    cout << curses::color::get_accent_color() << "Note" <<
                        curses::color::previous << ": " << _video_file["code"].get<string>() << endl;
                    continue;
                }
                providers::provider& provider = providers::provider::instance(_video_file["name"]);
                string url = _video_file["replace"];
                url = std::regex_replace(url, regex("#"), _video_file["code"].get<string>());
                if (url.find("http") == string::npos)
                    url = "http:" + url;
                video_files.insert({&provider, new video_file(provider, url, _video_file["text"])});
            }

            loaded = true;
        }
        
        string episode::get_id() const {
            return file(*this).get_id();
        }
        
        ostream& episode::print(ostream& stream) const {
            return stream << "Episode " << number;
        }
        
        unique_ptr<aggregators::episode::file> episode::get_file() const {
            return unique_ptr<aggregators::episode::file>(new episode::file(*this));
        }
    }
}