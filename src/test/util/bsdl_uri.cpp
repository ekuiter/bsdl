#define private public
#include "../test_util.hpp"
#include "../mock_app.hpp"
#include "../../util/bsdl_uri.hpp"

struct bsdl_uri_fixture : public settings_and_data_fixture {
    string aggregator = get_data("some_aggregator");
    string provider = get_data("some_provider");
    string subtitle = get_data("some_subtitle");
    string scheme = "bsdl://";
    string scheme_aggregator = scheme + aggregator;
    string valid_uri = scheme_aggregator + "/search";
    string complex_uri = valid_uri + "/series_url/bs_series_url?subtitles=" + subtitle + "#fragment";

    util::bsdl_uri get_data_uri(const string& uri) {
        return util::bsdl_uri(get_data(uri));
    }
};

struct complex_uri_fixture : public multi_fixture<bsdl_uri_fixture, silent_cout_fixture> {
    util::bsdl_uri complex_uri_obj;
    
    complex_uri_fixture(): complex_uri_obj(complex_uri) {}
};

bool is_none_uri(const util::uri_error& e) {
    return string(e.what()).find("no series at URI") != string::npos;
}

bool is_ambiguous_uri(const util::uri_error& e) {
    return string(e.what()).find("ambiguous series at URI") != string::npos;
}

BOOST_AUTO_TEST_SUITE(util_suite)
BOOST_FIXTURE_TEST_SUITE(bsdl_uri_suite, bsdl_uri_fixture)

BOOST_AUTO_TEST_CASE(parse_invalid_uris) {
    vector<string> uris = {
        "", "bsdl://", scheme + "invalid aggregator/series", scheme_aggregator, scheme_aggregator + "/",
        valid_uri + "/series_url/bs_series_url/invalid", valid_uri + "?=", valid_uri + "?key=",
        valid_uri + "?=value", valid_uri + "?invalid_key=bla", valid_uri + "?subtitles=" + subtitle + "&"
    };
    for (auto uri : uris) {
        BOOST_TEST_INFO(string("checking URI ") + uri);
        BOOST_CHECK_THROW(util::bsdl_uri u(uri), util::uri_error);
    }
}

BOOST_AUTO_TEST_CASE(parse_valid_uris) {
    vector<string> uris = {
        valid_uri, valid_uri + "/series_url", valid_uri + "/series_url/bs_series_url",
        valid_uri + "?", valid_uri + "?providers=" + provider, valid_uri + "?subtitles=" + subtitle,
        valid_uri + "?subtitles=" + subtitle + "&providers=" + provider,
        complex_uri
    };
    for (auto uri : uris) {
        silent_cout_fixture f;
        BOOST_TEST_INFO(string("checking URI ") + uri);
        util::bsdl_uri u(uri);
    }
}

BOOST_AUTO_TEST_CASE(decode_uri) { BOOST_TEST(util::bsdl_uri(valid_uri + "%20uri%2fencoded%24%3f").search_string == "search uri/encoded$?"); }

BOOST_FIXTURE_TEST_SUITE(complex_uri_suite, complex_uri_fixture)
BOOST_AUTO_TEST_CASE(member_aggregator) { BOOST_TEST(complex_uri_obj.aggregator == &aggregators::aggregator::instance(aggregator)); }
BOOST_AUTO_TEST_CASE(member_uri) { BOOST_TEST(complex_uri_obj.uri == complex_uri); }
BOOST_AUTO_TEST_CASE(member_search_string) { BOOST_TEST(complex_uri_obj.search_string == "search"); }
BOOST_AUTO_TEST_CASE(member_series_url) { BOOST_TEST(complex_uri_obj.series_url == "series_url"); }
BOOST_AUTO_TEST_CASE(member_bs_series_url) { BOOST_TEST(complex_uri_obj.bs_series_url == "bs_series_url"); }
BOOST_AUTO_TEST_CASE(params_present) { BOOST_TEST(complex_uri_obj.params["subtitles"] == subtitle); }
BOOST_AUTO_TEST_CASE(params_empty) { BOOST_TEST(complex_uri_obj.params["providers"] == ""); }
BOOST_AUTO_TEST_SUITE_END()

LONG_RUNNING_TEST_SUITE(AUTO, * utf::timeout(30))

BOOST_AUTO_TEST_CASE(fetch_series_valid) {
    mock_app::instance().series_search = get_data_uri("valid_bsdl_uri").get_search_string();
    BOOST_TEST(util::bsdl_uri(get_data_uri("valid_bsdl_uri").fetch_series()).get_uri() == get_data("valid_bsdl_uri"));
}

BOOST_AUTO_TEST_CASE(fetch_series_invalid_none) {
    BOOST_CHECK_EXCEPTION(get_data_uri("invalid_bsdl_uri_none").fetch_series(), util::uri_error, is_none_uri);
}

BOOST_AUTO_TEST_CASE(fetch_series_invalid_ambiguous) {
    BOOST_CHECK_EXCEPTION(get_data_uri("invalid_bsdl_uri_ambiguous").fetch_series(), util::uri_error, is_ambiguous_uri);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
