#include "merge_with_bs.hpp"
#include "../../app.hpp"
#include "../../settings.hpp"
#include "../../curses/terminal.hpp"
#include "../bs/bs.hpp"

namespace aggregators {    
    namespace mk {
        void merge_with_bs::fetch_source_series(aggregators::series& series) {
            curses::terminal::instance().get_stream(cout).set_visible(false);

            vector<aggregators::series*> search_results;
            auto preferred_aggregators = aggregator::get_preferred_aggregators();
            if (find(preferred_aggregators.begin(), preferred_aggregators.end(), &bs::bs::instance())
                    != preferred_aggregators.end()) {
                search_results = app::instance().get_search_results();
                search_results.erase(remove_if(search_results.begin(), search_results.end(), 
                    [](aggregators::series* series) { return &series->get_aggregator() != &bs::bs::instance(); }),
                    search_results.end());
            } else
                search_results = bs::bs::instance().search_internal(app::instance().get_series_search());
            search_results.insert(search_results.begin(), new bs::series(bs::bs::instance(), "(No episode titles)"));

            if (search_results.size() > 0)
                (src_series = &app::instance().choose_series(
                        search_results, "Use episode titles from:", "Choose"))->load();
            curses::terminal::instance().get_stream(cout).set_visible(true);
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
                    auto mk_episode = dynamic_cast<episode*>(dst_episode);
                    auto bs_episode = dynamic_cast<bs::episode*>(src_episode);
                    mk_episode->set_bs_episode(bs_episode);
            });
            curses::terminal::instance().get_stream(cout).set_visible(true);
        }
    }
}
