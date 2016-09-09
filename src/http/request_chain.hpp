#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include "request.hpp"
#include "response.hpp"

using namespace std;

namespace http {
    class request_chain {
    public:
        typedef function<request (const response&)> request_builder;

    private:
        const request initial_request;
        vector<request_builder> request_builders;

    public:
        request_chain(const request& _initial_request, const vector<request_builder>& _request_builders):
                initial_request(_initial_request), request_builders(_request_builders) {}

        response operator()() const;
    };
}
