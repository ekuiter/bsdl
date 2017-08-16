#include "test_util.hpp"
#include "../../http/download_queue.hpp"

struct mock_download {
    int i;
    http::request::download download_request;
    string file_name;
    
    mock_download(int _i, const http::request::download& request):
        i(_i), download_request(request), file_name(resource_file("download_queue_binary_" + to_string(i))) {}
    http::request::download& get_download_request() { return download_request.set_file_name(file_name); }
    void set_message(const string& _message) {}
};

BOOST_AUTO_TEST_SUITE(http_suite)
BOOST_AUTO_TEST_SUITE(download_queue_suite)

BOOST_AUTO_TEST_CASE(empty_download_queue, * utf::timeout(1)) {
    http::download_queue<mock_download> download_queue;
    download_queue();
}

LONG_RUNNING_TEST_SUITE(AUTO, * utf::timeout(10))

BOOST_FIXTURE_TEST_CASE(full_download_queue, data_fixture) {
    for (int transfers = 1; transfers <= 3; transfers++) {
        http::download_queue<mock_download> download_queue(transfers);
        for (int i = 0; i < 5; i++)
            download_queue.push(mock_download(i, http::request::download(get_data("some_binary"))));
        download_queue();
        for (auto download : download_queue) {
            check_file_type(download.file_name, get_data("binary_file_type"));
            remove(download.file_name.c_str());
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
