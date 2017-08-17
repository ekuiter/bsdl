#include "test_util.hpp"

BOOST_AUTO_TEST_SUITE(aggregators_suite)
BOOST_FIXTURE_TEST_SUITE(subtitle_suite, aggregator_fixture)

BOOST_AUTO_TEST_CASE(preferred_subtitle_empty) {
    BOOST_CHECK_THROW(aggregators::subtitle::get_preferred_subtitle({}), aggregators::exception);
}

BOOST_AUTO_TEST_CASE(preferred_subtitle_invalid) {
    settings_fixture f({ "-s", "GerSub" });
    BOOST_CHECK_THROW(aggregators::subtitle::get_preferred_subtitle({&aggregators::subtitle::instance("EngSub")}), aggregators::exception);
}

BOOST_FIXTURE_TEST_CASE(preferred_subtitle_single_valid, silent_cout_fixture) {
    settings_fixture f({ "-s", "GerSub" });
    aggregators::subtitle& subtitle = aggregators::subtitle::get_preferred_subtitle({&aggregators::subtitle::instance("GerSub")});
    BOOST_TEST(subtitle.get_name() == "GerSub");
}

BOOST_FIXTURE_TEST_CASE(preferred_subtitle_multiple_valid, silent_cout_fixture) {
    settings_fixture f({ "-s", "GerSub", "-s", "EngSub" });
    auto subtitles = {&aggregators::subtitle::instance("EngSub"), &aggregators::subtitle::instance("GerSub")};
    aggregators::subtitle& subtitle = aggregators::subtitle::get_preferred_subtitle(subtitles);
    BOOST_TEST(subtitle.get_name() == "GerSub");
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
