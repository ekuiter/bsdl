#include "test_util.hpp"

static int count_video_files(aggregators::episode* episode) {
    int video_files = 0;
    silent_cerr_fixture f;
    while (episode->next_preferred_video_file()->get_provider().get_name() != "Unavailable")
        video_files++;
    return video_files;
}

BOOST_AUTO_TEST_SUITE(aggregators_suite)
BOOST_AUTO_TEST_SUITE(episode_suite)
LONG_RUNNING_TEST_SUITE(FIXTURE, series_fixture, * utf::timeout(30))

BOOST_AUTO_TEST_CASE(episode_test) {
    BOOST_TEST_CHECKPOINT("episode_season_number");
    for (aggregators::season* season : series)
        for (aggregators::episode* episode : *season)
            BOOST_TEST(episode->get_season_number() == season->get_number());
    
    BOOST_TEST_CHECKPOINT("episode_series_title");
    for (aggregators::season* season : series)
        for (aggregators::episode* episode : *season)
            BOOST_TEST(episode->get_series_title() == series.get_title());

    BOOST_TEST_CHECKPOINT("episode_description");
    for (aggregators::season* season : series)
        for (aggregators::episode* episode : *season)
            BOOST_TEST(episode->get_description() != "");
}

BOOST_AUTO_TEST_CASE(episode_video_files) {
    aggregators::episode* episode = (*series[1])[1];
    int video_files = count_video_files(episode);
    BOOST_TEST(count_video_files(episode) == 0);
    episode->reset_skipped_video_files();
    BOOST_TEST(video_files == count_video_files(episode));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
