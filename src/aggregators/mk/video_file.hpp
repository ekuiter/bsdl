#pragma once

#include <iostream>
#include <Node.h>
#include "../../http/client.hpp"
#include "../../providers/provider.hpp"
#include "../video_file.hpp"

using namespace std;

namespace aggregators {
    namespace mk {
        class video_file : public aggregators::video_file {
        protected:
            vector<http::request> requests;
            void load_download_request() const override;
            
        public:
            video_file(const providers::provider& provider, const vector<http::request>& _requests):
                aggregators::video_file(provider, http::request::idle), requests(_requests) {}
        };
    }
}
