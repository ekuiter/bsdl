#pragma once

#include <iostream>
#include <sstream>
#include "series.hpp"
#include "exception.hpp"
#include <regex>
#include <algorithm>
#include <boost/format.hpp>

using namespace std;

namespace aggregators {
    class episode::file {
    protected:
        string old_file_name;
        mutable string file_name;
        const episode* _episode;
        
        static bool similar_file_exists(const string& file_name);

    public:
        file(const episode& episode): _episode(&episode) {}
        file(series& _series, const string& _old_file_name, string pattern_str = "");
        static void rename_files(series& _series, string directory_name, string pattern_str = "");

        const episode* get_episode() const {
            return _episode;
        }
        
        virtual string get_file_name() const = 0;
        static int get_number_of_seasons();

        static int get_season_digits() {
            int num = get_number_of_seasons(), digits = 0;
            for (; num != 0; num /= 10, digits++);
            return digits;
        }
    };
}
