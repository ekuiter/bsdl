#define BOOST_TEST_MODULE bsdltest
#include <boost/test/unit_test.hpp>
#include "test_util.hpp"

string executable_file() {
    return utf::framework::master_test_suite().argv[0];
}

string resource_file(const string& resource) {
    return settings::instance().resource_file({ executable_file() }, resource);
}
