#include "bs.hpp"
#include "exception.hpp"
#include "../curses/terminal.hpp"
#include "../app/settings.hpp"
#include <algorithm>
#include <functional>
#include <Node.h>
#include <boost/algorithm/string.hpp>

namespace bs {
    static int levenshtein(const string& a, const string& b) {
        int m = a.length(), n = b.length();
        int d[m + 1][n + 1];

        d[0][0] = 0;
        for (int i = 1; i <= m; i++)
            d[i][0] = i;
        for (int j = 1; j <= n; j++)
            d[0][j] = j;

        for (int i = 1; i <= m; i++)
            for (int j = 1; j <= n; j++)
                d[i][j] = min({
                        d[i - 1][j] + 1,
                        d[i][j - 1] + 1,
                        d[i - 1][j - 1] + (a[i] != b[j])
                });

        return d[m][n];
    }

    static int smith_waterman(const string& a, const string& b, function<int (char, char, int)> pred = nullptr) {
        pred = pred ? pred : [](char a, char b, int gap) {
            return a == b ? 2 : -1;
        };

        int m = a.length(), n = b.length(), gap = -1, h_max = 0;
        int h[m + 1][n + 1];

        for (int i = 0; i <= m; i++)
            h[i][0] = 0;
        for (int j = 0; j <= n; j++)
            h[0][j] = 0;

        for (int i = 1; i <= m; i++)
            for (int j = 1; j <= n; j++) {
                h[i][j] = max({
                        0,
                        h[i - 1][j - 1] + pred(a[i], b[j], gap),
                        h[i - 1][j] + pred(a[i], gap, gap),
                        h[i][j - 1] + pred(gap, b[j], gap)
                });
                h_max = max(h_max, h[i][j]);
            }

        return h_max;
    }

    static double get_string_similarity(const string& a, const string& b) {
        return (double) smith_waterman(a, b) / (2 * max(a.length(), b.length()));
    }

    http::request bs::root() {
        return http::request(settings::get("root_url"));
    }

    unique_ptr<vector<series>> bs::search(string series_search) {
        cout << "Searching for series " << curses::color::get_accent_color() <<
                series_search << curses::color::previous << "." << endl;
        unique_ptr<vector<series>> search_results(new vector<series>());
        boost::to_lower(series_search);

        unique_ptr<CDocument> document = root().get_relative(settings::get("search_path"))().parse();
        CSelection sel = document->find(settings::get("series_sel"));

        for (int i = 0; i < sel.nodeNum(); i++) {
            string current_series_title = sel.nodeAt(i).text();
            boost::to_lower(current_series_title);
            if (boost::contains(current_series_title, series_search) ||
                    get_string_similarity(current_series_title, series_search) > 0.5) {
                CNode series_node = sel.nodeAt(i).find("a").assertNum(1).nodeAt(0);
                search_results->push_back(series(
                        series_node.text(),
                        root().get_relative(series_node.attribute("href"))
                ));
            }
        }

        return search_results;
    }
}