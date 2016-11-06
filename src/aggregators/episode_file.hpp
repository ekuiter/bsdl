#pragma once

#include <iostream>
#include <sstream>
#include "series.hpp"
#include "exception.hpp"
#include <regex>
#include <algorithm>

using namespace std;

namespace aggregators {
    class episode::file {
    protected:
        string old_file_name;
        mutable string file_name;
        const episode* _episode;

    public:
        file(const episode& episode): _episode(&episode) {}
        file(series& _series, const string& _old_file_name, string pattern_str = "");
        static void rename_files(series& _series, string directory_name, string pattern_str = "");

        virtual string get_id() const = 0;
        virtual string get_file_name() const = 0;
    };
}
