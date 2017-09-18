#include "test_util.hpp"

struct episode_file_fixture : public aggregator_fixture {
    aggregators::series& series;
    unique_ptr<aggregators::episode::file> episode_file;

    episode_file_fixture(): series(any_series()), episode_file(any_episode_file()) {}
    
    aggregators::series& any_series() {
        string series_search;
        vector<aggregators::series*> search_results = bs_aggregator().search_internal(series_search = bs_series());
        return get_series_from_results(search_results, series_search, "bs");
    }
    
    unique_ptr<aggregators::episode::file> any_episode_file() {
        for (aggregators::season* season : series)
            for (aggregators::episode* episode : *season)
                return episode->get_file();
        throw runtime_error(string("no episode file found for series ") + series.get_title());
    }
};

BOOST_AUTO_TEST_SUITE(aggregators_suite)
BOOST_AUTO_TEST_SUITE(episode_file_suite)
LONG_RUNNING_TEST_SUITE(FIXTURE, episode_file_fixture, * utf::timeout(30))

BOOST_AUTO_TEST_CASE(episode_file_test) {
    BOOST_TEST_CHECKPOINT("episode_file_get_file_name");
    BOOST_TEST(episode_file->get_file_name() != "");

    BOOST_TEST_CHECKPOINT("episode_file_get_number_of_seasons");
    BOOST_TEST(aggregators::episode::file::get_number_of_seasons() == series.season_number());

    BOOST_TEST_CHECKPOINT("episode_file_default_pattern");
    BOOST_TEST(series.get_file("a file name with id 1.01", "default")->get_episode() == (*series[1])[1]);

    BOOST_TEST_CHECKPOINT("episode_file_SxxExx_pattern");
    BOOST_TEST(series.get_file("a file name with id S01E01", "SxxExx")->get_episode() == (*series[1])[1]);

    BOOST_TEST_CHECKPOINT("episode_file_default_pattern_invalid");
    BOOST_CHECK_THROW(series.get_file("a file name with id S01E01", "default")->get_episode(), aggregators::exception);

    BOOST_TEST_CHECKPOINT("episode_file_rename_files");
    episode_file->rename_files(series, resource_file(""));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
