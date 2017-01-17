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
    namespace bs {
        class bs : public aggregators::aggregator {
            bs(): aggregator(settings::get("aggregator_bs")) {}
   
        public:
            static bs& instance() {
                static bs instance;
                return instance;
            }
            
            vector<aggregators::series*> search_internal(const string& series_search) const override;
            
            static http::request root();
        };
    }
}
