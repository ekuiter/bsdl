#include "test_util.hpp"

BOOST_FIXTURE_TEST_SUITE(provider_suite, settings_and_data_fixture)
BOOST_AUTO_TEST_CASE(provider_valid) { BOOST_CHECK_NO_THROW(some_provider()); }
BOOST_AUTO_TEST_CASE(provider_invalid) { BOOST_CHECK_THROW(providers::provider::instance("abc.xyz", true), providers::provider::exception); }

BOOST_AUTO_TEST_SUITE(long_running, * utf::disabled())

BOOST_AUTO_TEST_CASE(youtube_dl_install)
{ BOOST_CHECK_NO_THROW(providers::provider::youtube_dl<1>::install_youtube_dl()); }

BOOST_AUTO_TEST_CASE(youtube_dl_update, * utf::depends_on("provider_suite/long_running/youtube_dl_install"))
{ BOOST_CHECK_NO_THROW(providers::provider::youtube_dl<1>::update_youtube_dl(youtube_dl_request())); }

BOOST_AUTO_TEST_CASE(youtube_dl_fetch, * utf::depends_on("provider_suite/long_running/youtube_dl_update"))
{ BOOST_TEST(youtube_dl_provider().fetch(youtube_dl_request()).get_url() == data["youtube_dl_result"].get<string>()); }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
