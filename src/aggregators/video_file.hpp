#pragma once

#include <iostream>
#include <Node.h>
#include "../http/client.hpp"
#include "../providers/provider.hpp"
#include "exception.hpp"

using namespace std;

namespace aggregators {
    class video_file {
    protected:
        const providers::provider& _provider;
        mutable http::request request;
        mutable http::request::download download_request;
        mutable bool loaded;
        
        virtual void load_download_request() const = 0;

    public:
        video_file(const providers::provider& provider, const http::request& _request):
                _provider(provider), request(_request), loaded(false) {}

        const providers::provider& get_provider() const {
            return _provider;
        }

        const http::request::download& get_download_request() const {
            load();
            return download_request;
        }

        void load() const {
            if (!loaded) {
                load_download_request();
                loaded = true;
            }
        }
        
        friend ostream& operator<<(ostream& stream, const video_file& video_file) {
            if (video_file.loaded)
                return stream << video_file.get_provider() << " at " << video_file.get_download_request();
            else
                return stream << video_file.get_provider();
        }
        
        class unavailable;
    };
    
    class video_file::unavailable : public video_file {
    protected:
        virtual void load_download_request() const override {}
        
    public:
        unavailable(): video_file(providers::provider::instance("Unavailable"), http::request()) {}
    };
}
