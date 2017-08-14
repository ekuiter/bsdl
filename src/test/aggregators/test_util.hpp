#pragma once

#include "../test_util.hpp"
#include "../../aggregators/aggregator.hpp"
#include "../../aggregators/bs/bs.hpp"
#include "../../aggregators/kx/kx.hpp"
#include "../../aggregators/mk/mk.hpp"
#include "../../aggregators/pr/pr.hpp"

#define GET_AGGREGATOR(name) \
    aggregators::aggregator& name##_aggregator() { return aggregators::name::name::instance(); }

struct aggregator_fixture : public settings_and_data_fixture {   
    aggregator_fixture() {
        static bool initialized;
        if (!initialized) {
            aggregators::aggregator::set_preferred_aggregators(settings::instance().get_preferred_aggregators());
            initialized = true;
        }
    }

    GET_AGGREGATOR(bs)
    GET_AGGREGATOR(kx)
    GET_AGGREGATOR(mk)
    GET_AGGREGATOR(pr)
};
