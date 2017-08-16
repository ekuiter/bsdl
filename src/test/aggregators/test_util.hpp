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

struct aggregator_fixture : public settings_and_data_fixture {
    GET_AGGREGATOR(bs)
    GET_AGGREGATOR(kx)
    GET_AGGREGATOR(mk)
    GET_AGGREGATOR(pr)

    aggregators::series* any_series() {
        uniform_int_distribution<int> distribution(1, 4);
        int num = distribution(random_engine());
        vector<aggregators::series*> search_results;
        string series_search;
        if (num == 1) search_results = bs_aggregator().search_internal(series_search = bs_series());
        if (num == 2) search_results = kx_aggregator().search_internal(series_search = kx_series());
        if (num == 3) search_results = mk_aggregator().search_internal(series_search = mk_series());
        if (num == 4) search_results = pr_aggregator().search_internal(series_search = pr_series());
        BOOST_TEST(search_results.size() > 0);
        mock_app::instance().series_search = series_search;
        return *search_results.begin();
    }
};

struct series_fixture : aggregator_fixture {
    aggregators::series* series;
    
    series_fixture(): series(any_series()) {}
};
