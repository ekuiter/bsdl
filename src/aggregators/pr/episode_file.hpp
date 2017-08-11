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

            string get_file_name() const override;

            virtual string get_id() const override {
                string format_string = string("%02d");
                return boost::str(boost::format(format_string) % _episode->get_number());
            }
        };
    }
}
