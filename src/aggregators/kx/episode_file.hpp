#pragma once

#include <iostream>
#include <sstream>
#include "../episode_file.hpp"
#include "series.hpp"
#include "exception.hpp"
#include <regex>
#include <algorithm>

using namespace std;

namespace aggregators {
    namespace kx {
        class episode::file : public aggregators::episode::file {
	    mutable const bs::episode* bs_episode = nullptr;
            
        public:
            using aggregators::episode::file::file;

            string get_file_name() const override;
        };
    }
}
