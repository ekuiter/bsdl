#pragma once

#include <iostream>
#include <memory>
#include "../../http/client.hpp"
#include "../../settings.hpp"
#include "../aggregator.hpp"
#include "series.hpp"

using namespace std;

namespace aggregators {
    namespace mk {
        class mk : public aggregators::aggregator {
            mk(): aggregator(settings::get("aggregator_mk")) {}
            
        public:
            static mk& instance() {
                static mk instance;
                return instance;
            }
            
            vector<aggregators::series*> search_internal(const string& series_search) const override;
            
            static http::request root();
        };
    }
}
