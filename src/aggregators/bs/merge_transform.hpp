#pragma once

#include <iostream>
#include "episode.hpp"
#include "../season_transform.hpp"
#include "season.hpp"

using namespace std;

namespace aggregators {    
    namespace bs {
        class mergeable_episode : public aggregators::episode {
        protected:
            ::aggregators::bs::episode* bs_episode = nullptr;

        public:
            using aggregators::episode::episode;
            
            const ::aggregators::bs::episode* get_bs_episode() const {
                return bs_episode;
            }
                    
            void set_bs_episode(::aggregators::bs::episode* _bs_episode) {
                bs_episode = _bs_episode;
            }
        };
        
        class merge_transform : public aggregators::season_transform {
            aggregators::series* src_series;
            
            merge_transform(): aggregators::season_transform(nullptr, nullptr), src_series(nullptr) {}
            
        public:
            static merge_transform& instance() {
                static merge_transform instance;
                return instance;
            }
            
            void fetch_source_series();            
            void operator()(aggregators::season* _dst_season);
        };
    }
}
