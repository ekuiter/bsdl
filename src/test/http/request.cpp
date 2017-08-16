#include "test_util.hpp"

BOOST_AUTO_TEST_SUITE(http_suite)
BOOST_AUTO_TEST_SUITE(request_suite)

BOOST_AUTO_TEST_CASE(valid_empty_request) { check_request(http::request(), "http://", "", "", false, "GET"); }
BOOST_AUTO_TEST_CASE(invalid_empty_request) { BOOST_CHECK_THROW(http::request(""), http::exception); }
BOOST_AUTO_TEST_CASE(valid_simple_url_request) { check_request(http::request("http://abc.xyz"), "http://abc.xyz/", "abc.xyz", "/", false, "GET"); }

BOOST_AUTO_TEST_CASE(valid_complex_url_request) {
    check_request(http::request("http://abc.xyz/a/path?with=a&query=and#fragment"),
                  "http://abc.xyz/a/path?with=a&query=and", "abc.xyz", "/a/path?with=a&query=and", false, "GET");
}

BOOST_AUTO_TEST_CASE(invalid_simple_url_request) { BOOST_CHECK_THROW(http::request("httq://abc.xyz"), http::exception); }
BOOST_AUTO_TEST_CASE(invalid_complex_url_request) { BOOST_CHECK_THROW(http::request("http:/\\/abc.xyz/a/path?with=a&query=and#fragment"), http::exception); }

BOOST_AUTO_TEST_CASE(https_url_post_request) {
    check_request(http::request("https://abc.xyz/", http::request::method::POST), "https://abc.xyz/", "abc.xyz", "/", true, "POST");
}

BOOST_AUTO_TEST_CASE(request_get_relative) {
    check_request(http::request("http://abc.xyz/a/path").get_relative("long/path"), "http://abc.xyz/a/long/path", "abc.xyz", "/a/long/path", false, "GET");
}

BOOST_AUTO_TEST_CASE(request_concatenate) {
    check_request(http::request("http://abc.xyz/a/path") + "long/path", "http://abc.xyz/a/pathlong/path", "abc.xyz", "/a/pathlong/path", false, "GET");
}

BOOST_AUTO_TEST_CASE(empty_request) {
    silent_cerr_fixture cerr_f;
    silent_clog_fixture clog_f;
    BOOST_CHECK_THROW(http::request()(), http::exception);
}

BOOST_AUTO_TEST_CASE(idle_request) { check_response(http::request::idle(), 0, ""); }

LONG_RUNNING_TEST_SUITE(AUTO, * utf::timeout(10))

BOOST_AUTO_TEST_CASE(simple_url_request) { check_response(http::request("http://abc.xyz")(), 200, "<html"); }
BOOST_AUTO_TEST_CASE(complex_url_request) { check_response(http::request("http://abc.xyz/a/path?with=a&query=and#fragment")(), 404, "<html"); }
BOOST_AUTO_TEST_CASE(https_url_post_request) { check_response(http::request("https://abc.xyz/", http::request::method::POST)(), 404, "<html"); }

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
