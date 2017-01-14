#pragma once

#include <iostream>
#include "series.hpp"
#include "download_selection.hpp"
#include <boost/algorithm/string.hpp>

using namespace std;

namespace aggregators {
    class aggregator {
        string name;
        static vector<aggregator*> preferred_aggregators;
        virtual vector<series*> search_internal(const string& series_search) const = 0;
        
    protected:
        aggregator(const string& _name): name(_name) {}
        
    public:        
        static aggregator& instance(const string& name);
        static void set_preferred_aggregators(const vector<aggregator*>& _preferred_aggregators);
        
        static const vector<aggregator*>& get_preferred_aggregators() {
            return preferred_aggregators;
        }
        
        const string& get_name() const {
            return name;
        }
        
        static vector<series*> search(string series_search) {
            cout << "Searching for series " << curses::color::get_accent_color() <<
                    series_search << curses::color::previous << "." << endl;
            boost::to_lower(series_search);
            vector<series*> search_results;
            for (auto& preferred_aggregator : preferred_aggregators) {
                auto current_search_results = preferred_aggregator->search_internal(series_search);
                search_results.insert(search_results.end(), current_search_results.begin(), current_search_results.end());
            }
            return search_results;
        }
    };
    
    ostream& operator<<(ostream& stream, const aggregator& aggregator);
}
