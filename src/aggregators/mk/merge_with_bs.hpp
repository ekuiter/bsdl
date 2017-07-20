#pragma once

#include <iostream>
#include "../season_transform.hpp"
#include "season.hpp"

using namespace std;

namespace aggregators {    
    namespace mk {
        class merge_with_bs : public aggregators::season_transform {
            aggregators::series* src_series;
            
            merge_with_bs(): aggregators::season_transform(nullptr, nullptr), src_series(nullptr) {}
            
        public:
            static merge_with_bs& instance() {
                static merge_with_bs instance;
                return instance;
            }
            
            void fetch_source_series(aggregators::series& series);            
            void operator()(season* _dst_season);
        };
    }
}
