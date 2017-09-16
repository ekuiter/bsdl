#pragma once

#include <iostream>
#include <memory>
#include <functional>
#include "headers.hpp"
#include "request.hpp"
#include "request_chain.hpp"
#include "download_queue.hpp"
#include "response.hpp"

using namespace std;

namespace http {
    class client {
        request::callback _callback;
        vector<request::hook*> hooks;
        bool verbose;
        int timeout = 60;

        client():
                _callback([](request::status, const request&, curl_off_t, curl_off_t, curl::curl_easy_exception*) { return 0; }),
                verbose(false) {}

    public:
        static client& instance() {
            static client instance;
            return instance;
        }

        request::callback get_callback() const {
            return _callback;
        }

        client& set_callback(request::callback callback) {
            _callback = callback;
            return *this;
        }

        client& set_verbose(bool _verbose) {
            verbose = _verbose;
            return *this;
        }

        client& set_timeout(int _timeout) {
            timeout = _timeout;
            return *this;
        }

        vector<request::hook*>& get_hooks() {
            return hooks;
        }

        client& add_hook(request::hook* hook) {
            hooks.push_back(hook);
            return *this;
        }

        void prepare(const request& request, request::implementation_base& implementation) const;
        bool perform(const request& request) const;
    };
}
