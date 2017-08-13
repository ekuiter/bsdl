#include "test_util.hpp"
#include "../aggregators/aggregator.hpp"

BOOST_FIXTURE_TEST_SUITE(settings_suite, settings_fixture)
BOOST_AUTO_TEST_CASE(read_invalid_key) { BOOST_CHECK_THROW(settings::get("invalid key"), runtime_error); }
BOOST_AUTO_TEST_CASE(read_valid_key) { settings::get("aggregators"); }
BOOST_AUTO_TEST_CASE(is_set_valid_key) { BOOST_TEST(settings::instance().is_set("aggregators")); }
BOOST_AUTO_TEST_CASE(is_set_invalid_key) { BOOST_TEST(!settings::instance().is_set("invalid key")); }
BOOST_AUTO_TEST_CASE(empty_download_selection) { BOOST_TEST(settings::instance().get_download_selection().size() == 0); }
BOOST_AUTO_TEST_CASE(with_preferred_aggregator) { BOOST_TEST(settings::instance().get_preferred_aggregators().size() > 0); }
BOOST_AUTO_TEST_CASE(with_preferred_provider) { BOOST_TEST(settings::instance().get_preferred_providers().size() > 0); }
BOOST_AUTO_TEST_CASE(with_preferred_subtitle) { BOOST_TEST(settings::instance().get_preferred_subtitles().size() > 0); }
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(settings_suite)

BOOST_AUTO_TEST_CASE(with_download_selection) {
    settings_fixture f({ "-d", "-d", "1" });
    BOOST_TEST(settings::instance().get_download_selection().size() == 2);
}

#define INVALID_TEST_CASE(type, switch)                                 \
    BOOST_AUTO_TEST_CASE(with_invalid_##type) { BOOST_CHECK_THROW(settings_fixture({ switch, "abc.xyz" }), runtime_error); }

#define PREFERRED_TEST_CASE(type, switch)                               \
    BOOST_FIXTURE_TEST_CASE(with_preferred_##type##s, data_fixture) {   \
        settings_fixture f({ switch, get_data("some_" #type), switch, get_data("another_" #type) }); \
        auto actual = settings::instance().get_preferred_##type##s();   \
        auto expected = { &some_##type(), &another_##type() };          \
        BOOST_TEST(actual == expected, tt::per_element());              \
    }

INVALID_TEST_CASE(aggregator, "-a")
INVALID_TEST_CASE(provider, "-p")
INVALID_TEST_CASE(subtitle, "-s")
PREFERRED_TEST_CASE(aggregator, "-a")
PREFERRED_TEST_CASE(provider, "-p")
PREFERRED_TEST_CASE(subtitle, "-s")

BOOST_AUTO_TEST_CASE(valid_usage) { settings_fixture({ "some series", "-o", ".", "-r", ".", "SxxExx", "-v", "-h" }); }

BOOST_AUTO_TEST_CASE(invalid_usages) {
    BOOST_CHECK_THROW(settings_fixture({ "some series", "-o", "-r", ".", "SxxExx", "-v", "-h" }), runtime_error);
    BOOST_CHECK_THROW(settings_fixture({ "some series", "--o", ".", "-r", ".", "SxxExx", "-v", "-h" }), runtime_error);
    BOOST_CHECK_THROW(settings_fixture({ "some series", "-o", ".", "-r", ".", "SxxExx", "-version", "-h" }), runtime_error);
    BOOST_CHECK_THROW(settings_fixture({ "some series", "invalid" }), runtime_error);
    BOOST_CHECK_THROW(settings_fixture({ "-d", "x" }), invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
