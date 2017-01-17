#include "merge_with_bs.hpp"
#include "../../app.hpp"
#include "../../settings.hpp"
#include "../../curses/terminal.hpp"
#include "../bs/bs.hpp"

namespace aggregators {    
    namespace kx {
        void merge_with_bs::fetch_source_series(aggregators::series& series) {
            if (settings::get("merge_with_bs") == "merge") {
                curses::terminal::instance().get_stream(cout).set_visible(false);
                vector<aggregators::series*> search_results = app::instance().get_search_results();
                search_results.erase(remove_if(search_results.begin(), search_results.end(), 
                        [](aggregators::series* series) { return &series->get_aggregator() != &bs::bs::instance(); }),
                        search_results.end());
                        
                if (search_results.size() > 0)
                    (src_series = &app::instance().choose_series(
                            search_results, "Use episode titles from:", "Choose"))->load();
                curses::terminal::instance().get_stream(cout).set_visible(true);
            }
        }
        
        void merge_with_bs::operator()(season* _dst_season) {
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
                    auto kx_episode = dynamic_cast<episode*>(dst_episode);
                    auto bs_episode = dynamic_cast<bs::episode*>(src_episode);
                    kx_episode->set_bs_episode(bs_episode);
            });
            curses::terminal::instance().get_stream(cout).set_visible(true);
        }
    }
}
