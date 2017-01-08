#pragma once

#include <iostream>
#include "series.hpp"
#include "download_selection.hpp"
#include <boost/algorithm/string.hpp>

using namespace std;

namespace aggregators {
    class aggregator {
        virtual vector<series*> search_internal(const string& series_search) const = 0;
        
    public:
        vector<series*> search(string series_search) const {
            cout << "Searching for series " << curses::color::get_accent_color() <<
                    series_search << curses::color::previous << "." << endl;
            boost::to_lower(series_search);
            return search_internal(series_search);
        }
    };
}
