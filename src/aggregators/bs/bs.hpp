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
            vector<aggregators::series*> search_internal(const string& series_search) const override;
   
        public:
            static bs& instance() {
                static bs instance;
                return instance;
            }
            
            static http::request root();
        };
    }
}
