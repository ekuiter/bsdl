#include "pr.hpp"
#include "exception.hpp"
#include "../../curses/terminal.hpp"
#include "../../settings.hpp"
#include <algorithm>
#include <functional>
#include <Node.h>
#include <boost/algorithm/string.hpp>

namespace aggregators {
    namespace pr {
        http::request pr::root() {
            return http::request(settings::get("pr_root_url"));
        }

        vector<aggregators::series*> pr::search_internal(const string& series_search) const {
            vector<aggregators::series*> search_results;
            unique_ptr<CDocument> document = root().get_relative(settings::get("pr_search_path") + series_search)().parse();
            CSelection sel = document->find(settings::get("pr_series_sel"));

            for (int i = 0; i < sel.nodeNum(); i++) {
                CNode title_node = sel.nodeAt(i).find(settings::get("pr_title_sel")).ASSERT_NUM(1).nodeAt(0);
                CSelection language_nodes = sel.nodeAt(i).find(settings::get("pr_language_sel"));
                search_results.push_back(new series(*this, title_node.text(),
                                                    language_nodes.nodeNum() > 1 ? "Ger" : "Eng",
                                                    root().get_relative(title_node.attribute("href")) + settings::get("pr_list_path")));
            }

            return search_results;
        }
    }
}
