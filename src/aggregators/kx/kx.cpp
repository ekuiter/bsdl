#include "kx.hpp"
#include "exception.hpp"
#include "../../curses/terminal.hpp"
#include "../../settings.hpp"
#include <algorithm>
#include <functional>
#include <regex>
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
                CNode title_node = sel.nodeAt(i).find(settings::get("kx_title_sel")).assertNum(1).nodeAt(0),
                        language_node = sel.nodeAt(i).find(settings::get("kx_language_sel")).assertNum(1).nodeAt(0);
                string title_text = title_node.text();
                smatch title_results, language_results;
                assert(regex_search(title_text, title_results, regex("^(.*?)(\\*.*subbed\\*.*)?$")));
                assert(regex_search(language_node.attribute("src"), language_results, regex("lng/(.*)\\.")));
                search_results.push_back(new series(
                        *this,
                        title_results[1],
                        title_results[2] == "" ? series::to_language_string(stoi(language_results[1])) : "Sub",
                        root().get_relative(title_node.attribute("href"))
                ));
            }

            return search_results;
        }
    }
}