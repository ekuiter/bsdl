#pragma once

#include "../test_util.hpp"
#include "../../aggregators/aggregator.hpp"
#include "../../aggregators/bs/bs.hpp"
#include "../../aggregators/kx/kx.hpp"
#include "../../aggregators/mk/mk.hpp"
#include "../../aggregators/pr/pr.hpp"
#include "../mock_app.hpp"

#define GET_AGGREGATOR(name) \
    aggregators::aggregator& name##_aggregator() { return aggregators::name::name::instance(); }

inline aggregators::series& get_series_from_results(const vector<aggregators::series*>& search_results,
                                                    const string& series_search, const string& aggregator) {
    BOOST_TEST_REQUIRE(search_results.size() > 0,
                       string("could not find series ") + series_search + " for aggregator " + aggregator + ", check data.json");
    mock_app& mock_app = mock_app::instance();
    mock_app.series_search = series_search;
    return *(mock_app.current_series = *search_results.begin());
}

#define BYTE_LIMIT (128 * 1024)

static int limited_http_callback(http::request::status status, const http::request& request,
                                 curl_off_t _now_bytes, curl_off_t _total_bytes, curl::curl_easy_exception* e) {
    return _now_bytes >= BYTE_LIMIT ? -1 : 0;
}

struct aggregator_fixture : public settings_and_data_fixture {
    GET_AGGREGATOR(bs)
    GET_AGGREGATOR(kx)
    GET_AGGREGATOR(mk)
    GET_AGGREGATOR(pr)
};

struct series_fixture : public aggregator_fixture {
    aggregators::series& series;
    
    series_fixture(): series(any_series()) {}

    aggregators::series& any_series() {
        vector<string> args = test_arguments();
        if (args.empty())
            args = { "bs", "kx", "mk", "pr" };
        uniform_int_distribution<int> distribution(0, args.size() - 1);
        string aggregator = args[distribution(random_engine())];
        vector<aggregators::series*> search_results;
        string series_search;
        if (aggregator == "bs") search_results = bs_aggregator().search_internal(series_search = bs_series());
        else if (aggregator == "kx") search_results = kx_aggregator().search_internal(series_search = kx_series());
        else if (aggregator == "mk") search_results = mk_aggregator().search_internal(series_search = mk_series());
        else if (aggregator == "pr") search_results = pr_aggregator().search_internal(series_search = pr_series());
        else throw runtime_error(aggregator + " is not a valid aggregator");
        return get_series_from_results(search_results, series_search, aggregator);
    }
};

struct downloadable_series_fixture : public aggregator_fixture {
    settings_fixture f;
    aggregators::series& series;
    
    downloadable_series_fixture(): f({ "-s", "EngSub" }), series(downloadable_series()) {}

    aggregators::series& downloadable_series() {
        vector<vector<string>> all_series_data = data["downloadable_series"];
        shuffle(all_series_data.begin(), all_series_data.end(), random_engine());
        vector<string> series_data = *all_series_data.begin();
        string aggregator_title = series_data[0];
        string series_title = series_data[1];
        boost::to_lower(series_title);
        aggregators::aggregator* aggregator;
        if (aggregator_title == "bs") aggregator = &bs_aggregator();
        else if (aggregator_title == "kx") aggregator = &kx_aggregator();
        else if (aggregator_title == "mk") aggregator = &mk_aggregator();
        else if (aggregator_title == "pr") aggregator = &pr_aggregator();
        else throw runtime_error(aggregator_title + " is not a valid aggregator");
        return get_series_from_results(aggregator->search_internal(series_title), series_title, aggregator_title);
    }
};
