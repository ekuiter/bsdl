#pragma once

#include <iostream>
#include <memory>
#include "../http/client.hpp"
#include "series.hpp"
#include "download_selection.hpp"
#include "episode_file.hpp"

using namespace std;

namespace bs {
    class bs {
    public:
        static http::request root();
        static unique_ptr<vector<series>> search(string series_search);
    };
}
