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
            return http::request(settings::get("root_url"));
        }

        vector<series*> bs::search(string series_search) {
            cout << "Searching for series " << curses::color::get_accent_color() <<
                    series_search << curses::color::previous << "." << endl;
            vector<series*> search_results;
            boost::to_lower(series_search);

            unique_ptr<CDocument> document = root().get_relative(settings::get("search_path"))().parse();
            CSelection sel = document->find(settings::get("series_sel"));

            for (int i = 0; i < sel.nodeNum(); i++) {
                string current_series_title = sel.nodeAt(i).text();
                boost::to_lower(current_series_title);
                if (boost::contains(current_series_title, series_search) ||
                        util::get_string_similarity(current_series_title, series_search) > 0.5) {
                    CNode series_node = sel.nodeAt(i).find("a").assertNum(1).nodeAt(0);
                    if (series_search == current_series_title)
                        search_results.clear();
                    search_results.push_back(new series(
                            series_node.text(),
                            root().get_relative(series_node.attribute("href"))
                    ));
                    if (series_search == current_series_title)
                        break;
                }
            }

            return search_results;
        }
    }
}