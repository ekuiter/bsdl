#include "test_util.hpp"

BOOST_AUTO_TEST_SUITE(http_suite)
BOOST_AUTO_TEST_SUITE(download_request_suite)

BOOST_FIXTURE_TEST_CASE(empty_download_request, silent_cerr_fixture) {
    auto file_name = resource_file("empty_download_request");
    auto request = http::request::download();
    BOOST_CHECK_THROW(request(file_name), http::exception);
    remove(file_name.c_str());
}

LONG_RUNNING_TEST_SUITE(AUTO, * utf::timeout(10))

BOOST_AUTO_TEST_CASE(download_request_invalid_file) { BOOST_CHECK_THROW(http::request::download("http://abc.xyz")(""), http::exception); }

BOOST_AUTO_TEST_CASE(download_request_valid_file) {
    auto file_name = resource_file("download_request_valid_file");
    auto request = http::request::download("http://abc.xyz");
    BOOST_TEST(request(file_name));
    {
        ifstream file(file_name);
        BOOST_TEST(file.good(), file_name + " can not be read");
        string line;
        getline(file, line);
        BOOST_TEST(line.find("html") != string::npos);
    }
    remove(file_name.c_str());
}

BOOST_FIXTURE_TEST_CASE(download_request_binary, data_fixture) {
    auto file_name = resource_file("download_request_binary");
    auto request = http::request::download(get_data("some_binary"));
    BOOST_TEST(request(file_name));
    check_file_type(file_name, get_data("binary_file_type"));
    remove(file_name.c_str());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
