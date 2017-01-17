#pragma once

#include <iostream>
#include <functional>
#include "aggregator.hpp"

using namespace std;

namespace aggregators {    
    class season_transform {
    protected:
        season* dst_season;
        season* src_season;
        
    public:
        season_transform(season* _dst_season, season* _src_season): dst_season(_dst_season), src_season(_src_season) {}
        
        void operator()(function<void (episode*, episode*)> transform_fn) {
            if (!dst_season || !src_season)
                return;
            
            episode* src_episode;
            for (auto dst_episode : *dst_season) {
                try {
                    src_episode = (*src_season)[dst_episode->get_number()];
                } catch (aggregators::exception) {
                    continue;
                }

                transform_fn(dst_episode, src_episode);
            }            
        }
    };
}
