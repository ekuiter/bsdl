#pragma once

#include <iostream>
#include <Node.h>
#include "../../http/client.hpp"
#include "../../providers/provider.hpp"
#include "../video_file.hpp"

using namespace std;

namespace aggregators {
    namespace kx {
        class video_file : public aggregators::video_file {
        protected:
            mutable int first_mirror;
            void load_download_request() const override;
            
        public:
            video_file(const providers::provider& provider, const http::request& request):
                aggregators::video_file(provider, request), first_mirror(-1) {}
        };
    }
}
