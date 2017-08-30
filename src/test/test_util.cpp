#define BOOST_TEST_MODULE bsdltest
#include <boost/test/unit_test.hpp>
#include "test_util.hpp"

string executable_file() {
    return utf::framework::master_test_suite().argv[0];
}

string resource_file(const string& resource) {
    return settings::instance().resource_file(resource);
}

vector<string> test_arguments() {
    utf::master_test_suite_t& suite = utf::framework::master_test_suite();
    vector<string> args(suite.argv, suite.argv + suite.argc);
    args.erase(args.begin());
    return args;
}

void check_file_type(const string& file_name, const string& file_type) {
    BOOST_TEST(util::platform::exec(string("file --mime-type ") + file_name).find(file_type) != string::npos);
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
