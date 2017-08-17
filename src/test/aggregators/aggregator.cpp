#include "test_util.hpp"

static void check_search_results(vector<aggregators::series*> search_results, string expected_title) {
    BOOST_TEST(search_results.size() > 0);
    BOOST_TEST(any_of(search_results.begin(), search_results.end(),
                      [&expected_title](aggregators::series* series) {
                          string actual_title = series->get_title();
                          boost::to_lower(actual_title);
                          boost::to_lower(expected_title);
                          return actual_title.find(expected_title) == 0;
                      }));
}

#define AGGREGATOR_SEARCH(name)                                         \
    BOOST_AUTO_TEST_CASE(name##_search) {                               \
        vector<string> args = test_arguments();                         \
        if (find(args.begin(), args.end(), #name) != args.end()) {      \
            auto series = name##_series();                              \
            check_search_results(name##_aggregator().search_internal(series), series); \
        }                                                               \
    }

BOOST_AUTO_TEST_SUITE(aggregators_suite)
BOOST_FIXTURE_TEST_SUITE(aggregator_suite, aggregator_fixture)

BOOST_AUTO_TEST_CASE(aggregator_valid) { aggregators::aggregator::instance(get_data("some_aggregator")); }
BOOST_AUTO_TEST_CASE(aggregator_invalid) { BOOST_CHECK_THROW(aggregators::aggregator::instance("invalid aggregator"), aggregators::exception); }

LONG_RUNNING_TEST_SUITE(AUTO, * utf::timeout(30))

BOOST_AUTO_TEST_CASE(aggregator_search_invalid) { BOOST_TEST(aggregators::aggregator::search("$ invalid series title ?").size() == 0); }
BOOST_AUTO_TEST_CASE(aggregator_search_valid) { BOOST_TEST(aggregators::aggregator::search(bs_series()).size() > 0); }

AGGREGATOR_SEARCH(bs)
AGGREGATOR_SEARCH(kx)
AGGREGATOR_SEARCH(mk)
AGGREGATOR_SEARCH(pr)

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
