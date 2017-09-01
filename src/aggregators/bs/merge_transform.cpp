#include "merge_transform.hpp"
#include "../../app.hpp"
#include "../../settings.hpp"
#include "../../curses/terminal.hpp"
#include "../bs/bs.hpp"

namespace aggregators {    
    namespace bs {
        string empty_series_title() {
            return "(No episode titles)";
        }

        const series* try_to_get_bs_series(aggregators::series& series) {
            auto _mergeable_series = dynamic_cast<mergeable_series*>(&series);
            if (_mergeable_series && _mergeable_series->get_bs_series() &&
                _mergeable_series->get_bs_series()->get_title() != aggregators::bs::empty_series_title())
                return _mergeable_series->get_bs_series();
            else
                return nullptr;
        }
        
        void merge_transform::fetch_source_series(const mergeable_series* _dst_series) {
            auto dst_series = const_cast<mergeable_series*>(_dst_series);
            if (dst_series->get_bs_series()) {
                src_series = const_cast<series*>(dst_series->get_bs_series());
                dynamic_cast<aggregators::series*>(src_series)->load();
                return;
            }
            
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
            search_results.insert(search_results.begin(), new series(bs::instance(), empty_series_title()));

            aggregators::series* _src_series = &app::instance().choose_series(search_results, "Use episode titles from:");
            _src_series->load();
            src_series = dynamic_cast<series*>(_src_series);
            dst_series->set_bs_series(src_series);
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
