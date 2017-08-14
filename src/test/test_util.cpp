#define BOOST_TEST_MODULE bsdltest
#include <boost/test/unit_test.hpp>
#include "test_util.hpp"

string executable_file() {
    return utf::framework::master_test_suite().argv[0];
}

string resource_file(const string& resource) {
    return settings::instance().resource_file({ executable_file() }, resource);
}

struct seed_fixture {
    static default_random_engine random_engine;
    
    seed_fixture() {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        random_engine = default_random_engine(seed);
    }
};

default_random_engine seed_fixture::random_engine;

default_random_engine& random_engine() {
    return seed_fixture::random_engine;
}

BOOST_GLOBAL_FIXTURE(seed_fixture);
