#pragma once

#include <iostream>
#include <Node.h>
#include "../../http/client.hpp"
#include "../../providers/provider.hpp"
#include "../video_file.hpp"

using namespace std;

namespace aggregators {
    namespace pr {
        class video_file : public aggregators::video_file {
            string message;

        protected:
            void load_download_request() const override;

        public:
            video_file(const providers::provider& provider, const http::request& _request, const string& _message):
                aggregators::video_file(provider, _request), message(_message) {}
        };
    }
}
