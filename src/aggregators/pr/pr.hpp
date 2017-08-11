#pragma once

#include <iostream>
#include <memory>
#include "../../http/client.hpp"
#include "../../settings.hpp"
#include "../aggregator.hpp"
#include "series.hpp"
#include "episode_file.hpp"

using namespace std;

namespace aggregators {
    namespace pr {
        class pr : public aggregators::aggregator {
            pr(): aggregator(settings::get("aggregator_pr")) {}
   
        public:
            static pr& instance() {
                static pr instance;
                return instance;
            }
            
            vector<aggregators::series*> search_internal(const string& series_search) const override;
            
            static http::request root();
        };
    }
}
