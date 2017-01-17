#pragma once

#include <iostream>
#include <memory>
#include "../../http/client.hpp"
#include "../../settings.hpp"
#include "../aggregator.hpp"
#include "series.hpp"

using namespace std;

namespace aggregators {
    namespace kx {
        class kx : public aggregators::aggregator {
            kx(): aggregator(settings::get("aggregator_kx")) {}
            
        public:
            static kx& instance() {
                static kx instance;
                return instance;
            }
            
            vector<aggregators::series*> search_internal(const string& series_search) const override;
            
            static http::request root();
        };
    }
}
