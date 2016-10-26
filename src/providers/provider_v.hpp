#pragma once

#include "provider.hpp"
#include "provider_youtube_dl.hpp"
#include "../settings.hpp"
#include <iostream>

using namespace std;

namespace providers {
    class provider::v : public provider::youtube_dl {
        v(): youtube_dl(settings::get("provider_v")) {}

    public:
        static v& instance() {
            static v instance;
            return instance;
        }

        string get_file_format() const override {
            return settings::get("provider_v_file_format");
        }
    };
}
