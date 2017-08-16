#include "test_util.hpp"

BOOST_AUTO_TEST_SUITE(aggregators_suite)
BOOST_AUTO_TEST_SUITE(series_suite)
LONG_RUNNING_TEST_SUITE(FIXTURE, series_fixture, * utf::timeout(30))

BOOST_AUTO_TEST_CASE(series_test) {
    BOOST_TEST_CHECKPOINT("series_season_number");
    BOOST_TEST(series.season_number() > 0);
    int seasons = 0;
    for (aggregators::season* season : series)
        seasons++;
    BOOST_TEST(series.season_number() == seasons);
    
    BOOST_TEST_CHECKPOINT("series_season_access");
    for (aggregators::season* season : series)
        BOOST_TEST(series[season->get_number()] == season);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
