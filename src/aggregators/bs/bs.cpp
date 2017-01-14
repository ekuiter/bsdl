#include "bs.hpp"
#include "exception.hpp"
#include "../../curses/terminal.hpp"
#include "../../util/string_similarity.hpp"
#include "../../settings.hpp"
#include <algorithm>
#include <functional>
#include <Node.h>
#include <boost/algorithm/string.hpp>

namespace aggregators {
    namespace bs {
        http::request bs::root() {
            return http::request(settings::get("bs_root_url"));
        }

        vector<aggregators::series*> bs::search_internal(const string& series_search) const {
            vector<aggregators::series*> search_results;
            unique_ptr<CDocument> document = root().get_relative(settings::get("bs_search_path"))().parse();
            CSelection sel = document->find(settings::get("bs_series_sel"));

            for (int i = 0; i < sel.nodeNum(); i++) {
                string current_series_title = sel.nodeAt(i).text();
                boost::to_lower(current_series_title);
                if (boost::contains(current_series_title, series_search) ||
                        util::get_string_similarity(current_series_title, series_search) > 0.5) {
                    CNode series_node = sel.nodeAt(i).find("a").assertNum(1).nodeAt(0);
                    search_results.push_back(new series(
                            *this,
                            series_node.text(),
                            root().get_relative(series_node.attribute("href"))
                    ));
                }
            }

            return search_results;
        }
    }
}