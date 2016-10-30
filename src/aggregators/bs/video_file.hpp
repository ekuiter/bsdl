#pragma once

#include <iostream>
#include <Node.h>
#include "../../http/client.hpp"
#include "../../providers/provider.hpp"
#include "../video_file.hpp"

using namespace std;

namespace aggregators {
    namespace bs {
        class video_file : public aggregators::video_file {
            using aggregators::video_file::video_file;

        protected:
            void load_download_request() const override;
        };
    }
}
