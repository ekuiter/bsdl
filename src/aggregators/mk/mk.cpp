#include "mk.hpp"
#include "exception.hpp"
#include "../../curses/terminal.hpp"
#include "../../settings.hpp"
#include <algorithm>
#include <functional>
#include <regex>
#include <Node.h>
#include <boost/algorithm/string.hpp>

namespace aggregators {
    namespace mk {
        http::request mk::root() {
            return http::request(settings::get("mk_root_url"));
        }

        vector<aggregators::series*> mk::search_internal(const string& series_search) const {
            vector<aggregators::series*> search_results;
            unique_ptr<CDocument> document = root().get_relative(settings::get("mk_search_path") + series_search)().parse();
            CSelection sel = document->find(settings::get("mk_series_sel"));
            
            for (int i = 0; i < sel.nodeNum(); i++) {
                CNode title_node = sel.nodeAt(i).find(settings::get("mk_title_sel")).ASSERT_NUM(1).nodeAt(0),
                    language_node = sel.nodeAt(i).find(settings::get("mk_language_sel")).ASSERT_NUM(1).nodeAt(0);
                string title_text = title_node.text(), language_src = language_node.attribute("src"),
                    link = title_node.attribute("href");
                smatch results;
                if (regex_search(link, results, regex(settings::get("mk_exclude_regex"))) ||
                    title_node.attribute("target") != "")
                    continue;
                boost::trim(title_text);
                search_results.push_back(new series(*this, title_text, series::to_language_string(language_src),
                                                    root().get_relative(link)));
            }

            return search_results;
        }
    }
}
