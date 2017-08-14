#include "test_util.hpp"

struct response_fixture {
    static unique_ptr<CDocument> doc;

    response_fixture() {
        if (!doc)
            doc = http::request("http://abc.xyz")().parse();
    }
};

unique_ptr<CDocument> response_fixture::doc;

BOOST_AUTO_TEST_SUITE(http_suite)
BOOST_FIXTURE_TEST_SUITE(response_suite, response_fixture)

BOOST_AUTO_TEST_SUITE(long_running, * utf::disabled() * utf::timeout(10))

BOOST_AUTO_TEST_CASE(response_parse) {}
BOOST_AUTO_TEST_CASE(response_find_valid) { doc->find("head").ASSERT_NUM(1).find("meta").ASSERT_AT_LEAST(1); }
BOOST_AUTO_TEST_CASE(response_find_invalid) { BOOST_CHECK_THROW(doc->find("invalid tag").ASSERT_NUM(1), runtime_error); }

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
