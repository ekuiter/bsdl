#pragma once

#include <iostream>
#include <Node.h>
#include "../http/client.hpp"
#include "../app/provider.hpp"

using namespace std;

namespace bs {
    class video_file {
        const provider& _provider;
        http::request request;
        mutable http::request::download download_request;
        mutable bool loaded;

    public:
        static video_file unavailable_video_file;

        video_file(const provider& provider, const http::request& _request):
                _provider(provider), request(_request), loaded(false) {}

        const provider& get_provider() const {
            return _provider;
        }

        const http::request::download& get_download_request() const {
            load();
            return download_request;
        }

        void load() const;
        friend ostream& operator<<(ostream& stream, const video_file& video_file);
    };

    ostream& operator<<(ostream& stream, const video_file& video_file);
}
