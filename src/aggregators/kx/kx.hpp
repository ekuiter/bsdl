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
            vector<aggregators::series*> search_internal(const string& series_search) const override;
            
        public:
            static kx& instance() {
                static kx instance;
                return instance;
            }
            
            static http::request root();
        };
    }
}
