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
    namespace pr {
        class episode::file : public aggregators::episode::file {
        public:
            using aggregators::episode::file::file;
            file(series& _series, const string& _old_file_name, string pattern_str);

            string get_file_name() const override;
        };
    }
}
