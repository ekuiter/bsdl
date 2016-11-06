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
    namespace bs {
        class episode::file : public aggregators::episode::file {
            int get_season_digits() const {
                int num = get_number_of_seasons(), digits = 0;
                for (; num != 0; num /= 10, digits++);
                return digits;
            }

        public:
            using aggregators::episode::file::file;

            string get_id() const override {
                string format_string = string("%0") + to_string(get_season_digits()) + "d.%02d";
                return boost::str(boost::format(format_string) % _episode->get_season_number() % _episode->get_number());
            }

            string get_file_name() const override;
            int get_number_of_seasons() const;
        };
    }
}
