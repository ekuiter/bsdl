#pragma once

#include <iostream>
#include <memory>
#include <Document.h>
#include "headers.hpp"
#include "request.hpp"

using namespace std;

namespace http {
    class response {
        unsigned int status;
        string body;
        headers _headers;

    public:
        response(): status(0) {}
        response(const unsigned int _status, const string& _body, const headers& headers):
                status(_status), body(_body), _headers(headers) {}

        unsigned int get_status() const noexcept {
            return status;
        }

        const string& get_body() const noexcept {
            return body;
        }

        const headers& get_headers() const noexcept {
            return _headers;
        }

        unique_ptr<CDocument> parse() const;
    };

    ostream& operator<<(ostream& stream, const response& response);
}