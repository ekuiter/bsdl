#include "test_util.hpp"

BOOST_AUTO_TEST_SUITE(aggregators_suite)
BOOST_AUTO_TEST_SUITE(season_suite)
LONG_RUNNING_TEST_SUITE(FIXTURE, series_fixture, * utf::timeout(30))

BOOST_AUTO_TEST_CASE(season_test) {
    BOOST_TEST_CHECKPOINT("season_episode_number");
    for (aggregators::season* season : series) {
        BOOST_TEST(season->episode_number() > 0);
        int episodes = 0;
        for (aggregators::episode* episode : *season)
            episodes++;
        BOOST_TEST(season->episode_number() == episodes);
    }

    BOOST_TEST_CHECKPOINT("season_episode_access");
    for (aggregators::season* season : series)
        for (aggregators::episode* episode : *season)
            BOOST_TEST((*season)[episode->get_number()] == episode);
 
    BOOST_TEST_CHECKPOINT("season_series_title");
    for (aggregators::season* season : series)
        BOOST_TEST(season->get_series_title() == series.get_title());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
