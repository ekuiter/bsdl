#include "kx.hpp"
#include "exception.hpp"
#include "../../curses/terminal.hpp"
#include "../../settings.hpp"
#include <algorithm>
#include <functional>
#include <Node.h>
#include <boost/algorithm/string.hpp>

namespace aggregators {
    namespace kx {
        http::request kx::root() {
            return http::request(settings::get("kx_root_url"));
        }

        vector<aggregators::series*> kx::search_internal(const string& series_search) const {
            vector<aggregators::series*> search_results;
            unique_ptr<CDocument> document = root().get_relative(settings::get("kx_search_path") + series_search)().parse();
            CSelection sel = document->find(settings::get("kx_series_sel"));
            
            for (int i = 0; i < sel.nodeNum(); i++) {
                CNode series_node = sel.nodeAt(i).find(settings::get("kx_title_sel")).assertNum(1).nodeAt(0);
                search_results.push_back(new series(
                        series_node.text(),
                        root().get_relative(series_node.attribute("href"))
                ));
            }

            return search_results;
        }
    }
}