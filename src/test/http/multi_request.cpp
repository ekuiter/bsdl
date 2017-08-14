#include "test_util.hpp"

BOOST_AUTO_TEST_SUITE(http_suite)
BOOST_AUTO_TEST_SUITE(multi_request_suite)

BOOST_AUTO_TEST_CASE(empty_multi_request, * utf::timeout(1)) {
    http::multi::request multi_request;
    while (multi_request());
}

BOOST_AUTO_TEST_SUITE(long_running, * utf::disabled() * utf::timeout(10))

BOOST_AUTO_TEST_CASE(multi_request) {
    http::multi::request multi_request;
    auto requests = { http::request("http://abc.xyz"), http::request("http://google.com"), http::request("http://example.com") };
    for (auto request : requests)
        multi_request.add(request);
    while (multi_request());
}

BOOST_FIXTURE_TEST_CASE(download_multi_request, data_fixture) {
    http::multi::request::download download_multi_request;
    auto some_request = http::request::download(get_data("some_binary")),
        another_request = http::request::download(get_data("some_binary"));
    auto some_file_name = resource_file("download_multi_request_some_binary"),
        another_file_name = resource_file("download_multi_request_another_binary");
    some_request.set_file_name(some_file_name);
    another_request.set_file_name(another_file_name);
    download_multi_request.add(some_request);
    download_multi_request.add(another_request);
    while (download_multi_request());
    check_file_type(some_file_name, get_data("binary_file_type"));
    check_file_type(another_file_name, get_data("binary_file_type"));
    remove(some_file_name.c_str());
    remove(another_file_name.c_str());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
