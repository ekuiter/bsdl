#pragma once

#include <iostream>
#include <sstream>
#include "series.hpp"
#include "exception.hpp"
#include <regex>
#include <algorithm>

using namespace std;

namespace aggregators {
    namespace bs {
        class episode::file {
            string old_file_name;
            mutable string file_name;
            const episode* _episode;

            int get_season_digits() const {
                int num = get_number_of_seasons(), digits = 0;
                for (; num != 0; num /= 10, digits++);
                return digits;
            }

        public:
            file(const episode& episode): _episode(&episode) {}
            file(series& _series, const string& _old_file_name, string pattern_str = "");
            static void rename_files(series& _series, string directory_name, string pattern_str = "");

            string get_id() const {
                string format_string = string("%0") + to_string(get_season_digits()) + "d.%02d";
                return boost::str(boost::format(format_string) % _episode->get_season_number() % _episode->get_number());
            }

            string get_file_name() const;
            int get_number_of_seasons() const;
        };
    }
}
