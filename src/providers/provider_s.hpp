#pragma once

#include "provider.hpp"
#include "provider_youtube_dl.hpp"
#include "../settings.hpp"
#include <iostream>

using namespace std;

namespace providers {
    class provider::s : public provider::youtube_dl {
        s(): youtube_dl(get_provider_name("provider_s")) {}

    public:
        static s& instance() {
            static s instance;
            return instance;
        }

        string get_file_format() const override {
            return settings::get("provider_s_file_format");
        }
    };
}
