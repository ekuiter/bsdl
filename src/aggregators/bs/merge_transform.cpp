#include "merge_transform.hpp"
#include "../../app.hpp"
#include "../../settings.hpp"
#include "../../curses/terminal.hpp"
#include "../bs/bs.hpp"

namespace aggregators {    
    namespace bs {
        void merge_transform::fetch_source_series() {
            curses::terminal::instance().get_stream(cout).set_visible(false);

            vector<aggregators::series*> search_results;
            if (!app::instance().is_testing()) {
                search_results = app::instance().get_search_results();
                search_results.erase(remove_if(search_results.begin(), search_results.end(), 
                    [](aggregators::series* series) { return &series->get_aggregator() != &bs::instance(); }),
                    search_results.end());
            }
            if (search_results.size() == 0)
                search_results = bs::instance().search_internal(app::instance().get_series_search());
            search_results.insert(search_results.begin(), new series(bs::instance(), "(No episode titles)"));

            if (search_results.size() > 0)
                (src_series = &app::instance().choose_series(
                        search_results, "Use episode titles from:", "Choose"))->load();
            curses::terminal::instance().get_stream(cout).set_visible(true);
        }
        
        void merge_transform::operator()(aggregators::season* _dst_season) {
            if (!src_series || !_dst_season)
                return;

            dst_season = _dst_season;
            try {
                src_season = (*src_series)[dst_season->get_number()];
            } catch (aggregators::exception) {
                return;
            }

            curses::terminal::instance().get_stream(cout).set_visible(false);
            aggregators::season_transform::operator()
                ([](aggregators::episode* dst_episode, aggregators::episode* src_episode) {
                    auto _mergeable_episode = dynamic_cast<mergeable_episode*>(dst_episode);
                    auto bs_episode = dynamic_cast<episode*>(src_episode);
                    _mergeable_episode->set_bs_episode(bs_episode);
            });
            curses::terminal::instance().get_stream(cout).set_visible(true);
        }
    }
}
