#pragma once

#include <iostream>
#include "series.hpp"
#include "download_selection.hpp"

using namespace std;

namespace aggregators {
    class aggregator {
    public:
        virtual vector<series*> search(string series_search) const = 0;
    };
}
