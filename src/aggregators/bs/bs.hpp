#pragma once

#include <iostream>
#include <memory>
#include "../../http/client.hpp"
#include "../aggregator.hpp"
#include "series.hpp"
#include "download_selection.hpp"
#include "episode_file.hpp"

using namespace std;

namespace aggregators {
    namespace bs {
        class bs : public aggregators::aggregator {
            bs() {}
            
        public:
            static bs& instance() {
                static bs instance;
                return instance;
            }
            
            static http::request root();
            vector<aggregators::series*> search(string series_search) const override;
        };
    }
}
