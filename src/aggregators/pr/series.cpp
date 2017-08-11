#include "series.hpp"
#include "pr.hpp"
#include "../../curses/terminal.hpp"
#include "../../settings.hpp"
#include <Node.h>

namespace aggregators {
    namespace pr {
        vector<aggregators::subtitle*> series::get_subtitles(const http::response& response) {
            unique_ptr<CDocument> document = response.parse();
            CSelection subtitle_sel = document->find(settings::get("pr_subtitle_sel")).ASSERT_AT_LEAST(1);
            vector<aggregators::subtitle*> subtitles;
            for (int i = 0; i < subtitle_sel.nodeNum(); i++) {
                CNode subtitle_node = subtitle_sel.nodeAt(i);
                if (subtitle_node.text().find(settings::get("pr_subtitle_key")) != string::npos)
                    subtitles.push_back(&aggregators::subtitle::instance(subtitle_node.childAt(0).text()));
            }
            return subtitles;
        }
        
        void series::load(const http::response& response) const {
            aggregators::subtitle& subtitle = aggregators::subtitle::get_preferred_subtitle(get_subtitles(response));
            unique_ptr<CDocument> document = response.parse();
            CSelection season_nodes = document->find(settings::get("pr_season_sel"));
            int season_number = 1;

            if (season_nodes.nodeNum() > 0)
                for (auto& season_node : season_nodes) {
                    string season_text = CNode(season_node).text();
                    string season_url = CNode(season_node).attribute("href");
                    http::request season_request(pr::root().get_relative(season_url));
                    add_season(season_number == 1 ?
                               new season(title, 1, response, season_text, subtitle) :
                               new season(title, season_number, season_request, season_text, subtitle));
                    season_number++;
                }
            else
                add_season(new season(title, 1, response, title, subtitle));

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
