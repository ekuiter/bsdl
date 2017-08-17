#include "test_util.hpp"

struct video_file_fixture : public downloadable_series_fixture {
    const aggregators::video_file& video_file;
    
    video_file_fixture(): video_file(any_video_file()) {}
    
    const aggregators::video_file& any_video_file() {
        const aggregators::video_file* video_file;
        for (aggregators::season* season : series)
            for (aggregators::episode* episode : *season) {
                silent_cerr_fixture f;
                if ((video_file = episode->next_preferred_video_file())->get_provider().get_name() != "Unavailable")
                    return *video_file;
            }
        throw runtime_error(string("no video file found for series ") + series.get_title());
    }
};

BOOST_AUTO_TEST_SUITE(aggregators_suite)
BOOST_AUTO_TEST_SUITE(video_file_suite)
LONG_RUNNING_TEST_SUITE(FIXTURE, video_file_fixture, * utf::timeout(30))

BOOST_AUTO_TEST_CASE(video_file_test) {    
    BOOST_TEST_CHECKPOINT("video_file_provider");
    BOOST_TEST(video_file.get_provider().get_name() != "Unavailable");
    
    BOOST_TEST_CHECKPOINT("video_file_load");
    video_file.load();

    BOOST_TEST_CHECKPOINT("video_file_download_request");
    http::request::download request = video_file.get_download_request();
    BOOST_TEST(!request.is_empty());

    BOOST_TEST_CHECKPOINT("video_file_download_request_binary");
    auto file_name = resource_file("video_file_download_request_binary");
    request.set_callback(limited_http_callback);
    request(file_name);
    check_file_type(file_name, video_file.get_provider().get_file_format());
    remove(file_name.c_str());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
