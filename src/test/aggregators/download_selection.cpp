#include "test_util.hpp"

struct download_selection_fixture : aggregator_fixture {
    aggregators::series* series;
    
    download_selection_fixture() {
        auto search_results = bs_aggregator().search_internal(bs_series());
        BOOST_TEST(search_results.size() > 0);
        series = *search_results.begin();
    }

    aggregators::download_selection& get_download_selection() {
        return settings::instance().get_download_selection();
    }
    
    int series_episode_number() {
        int num = 0;
        for (auto season : *series)
            num += season->episode_number();
        return num;
    }
};

#define DOWNLOAD_SELECTOR(type, args, episode_number, selector_string)  \
    BOOST_AUTO_TEST_CASE(download_selector_##type) {                    \
        settings_fixture f args;                                        \
        BOOST_TEST(get_download_selection().size() == 1);               \
        BOOST_TEST(get_download_selection().get_episodes(*series).size() == episode_number); \
        for (auto download_selector : get_download_selection())         \
            BOOST_TEST((string) *download_selector == selector_string); \
    }

BOOST_AUTO_TEST_SUITE(aggregators_suite)
BOOST_AUTO_TEST_SUITE(download_selection_suite)

BOOST_FIXTURE_TEST_SUITE(long_running, download_selection_fixture, * utf::disabled() * utf::timeout(30))

DOWNLOAD_SELECTOR(series, ({ "-d" }), series_episode_number(), "series")
DOWNLOAD_SELECTOR(season, ({ "-d", "1" }), (*series)[1]->episode_number(), "season(1)")
DOWNLOAD_SELECTOR(episode, ({ "-d", "1", "1" }), 1, "episode(1, 1)")
DOWNLOAD_SELECTOR(latest_episode, ({ "-d", "latest" }), 1, "latest_episode")

BOOST_AUTO_TEST_CASE(empty_download_selection) {
    BOOST_TEST(get_download_selection().size() == 0);
    BOOST_TEST(get_download_selection().get_episodes(*series).size() == 0);
}

BOOST_AUTO_TEST_CASE(mixed_download_selection) {
    settings_fixture f({ "-d", "-d", "1", "-d", "1", "1", "-d", "latest" });
    BOOST_TEST(get_download_selection().size() == 4);
    BOOST_TEST(get_download_selection().get_episodes(*series).size() == series_episode_number());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
