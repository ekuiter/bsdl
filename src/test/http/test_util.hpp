#pragma once

#include "../test_util.hpp"

inline void check_request(const http::request& request, const string& url, const string& host,
                          const string& path, bool https, const string& method) {
    BOOST_TEST(request.get_url() == url);
    BOOST_TEST(request.get_host() == host);
    BOOST_TEST(request.get_path() == path);
    BOOST_TEST(request.get_https() == https);
    BOOST_TEST(request.get_method_string() == method);
}

inline void check_response(const http::response& response, unsigned int status, const string& excerpt) {
    BOOST_TEST(response.get_status() == status);
    BOOST_TEST(response.get_body().find(excerpt) != string::npos);
}

