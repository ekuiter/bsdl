#pragma once

#include <iostream>
#include "series.hpp"
#include "season.hpp"
#include "episode.hpp"
#include "../season_transform.hpp"

using namespace std;

namespace aggregators {    
    namespace bs {
        class mergeable_series : public aggregators::series {
        protected:
            ::aggregators::bs::series* bs_series = nullptr;

        public:
            using aggregators::series::series;
            
            const ::aggregators::bs::series* get_bs_series() const {
                return bs_series;
            }
                    
            void set_bs_series(::aggregators::bs::series* _bs_series) {
                bs_series = _bs_series;
            }
        };

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
            aggregators::bs::series* src_series = nullptr;
            
            merge_transform(): aggregators::season_transform(nullptr, nullptr) {}
            
        public:
            static merge_transform& instance() {
                static merge_transform instance;
                return instance;
            }
            
            void fetch_source_series(const mergeable_series* dst_series);            
            void operator()(aggregators::season* _dst_season);
        };

        string empty_series_title();
        const series* try_to_get_bs_series(aggregators::series& series);
    }
}
