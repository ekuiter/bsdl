#include "test_util.hpp"

struct episode_download_fixture : public downloadable_series_fixture {
    unique_ptr<aggregators::episode::download> episode_download;
    
    episode_download_fixture(): episode_download(any_episode_download()) {}
    
    unique_ptr<aggregators::episode::download> any_episode_download() {
        for (aggregators::season* season : series)
            for (aggregators::episode* episode : *season)
                return episode->get_download();
        throw runtime_error(string("no episode download found for series ") + series.get_title());
    }
};

BOOST_AUTO_TEST_SUITE(aggregators_suite)
BOOST_AUTO_TEST_SUITE(episode_download_suite)
LONG_RUNNING_TEST_SUITE(FIXTURE, episode_download_fixture, * utf::timeout(30))

BOOST_AUTO_TEST_CASE(episode_download_test) {
    BOOST_TEST_CHECKPOINT("episode_file_get_file_name");
    BOOST_TEST(episode_download->get_file_name() != "");

    BOOST_TEST_CHECKPOINT("episode_file_get_file_path");
    BOOST_TEST(episode_download->get_file_path(resource_file("")) != "");

    BOOST_TEST_CHECKPOINT("episode_file_download_request");
    http::request::download request = episode_download->get_download_request();
    BOOST_TEST(!request.is_empty());

    BOOST_TEST_CHECKPOINT("episode_file_download_request_binary");
    auto file_name = resource_file("episode_file_download_request_binary");
    request.set_callback(limited_http_callback);
    request(file_name);
    check_file_type(file_name, episode_download->get_video_file()->get_provider().get_file_format());
    remove(file_name.c_str());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
