#define BOOST_TEST_MODULE bsdltest
#include <boost/test/unit_test.hpp>
#include "test_util.hpp"

string executable_file() {
    return utf::framework::master_test_suite().argv[0];
}
