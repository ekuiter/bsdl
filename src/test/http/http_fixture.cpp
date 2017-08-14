#include "test_util.hpp"

struct http_fixture {    
    http_fixture() {
        if (http::request("http://abc.xyz")().get_status() != 200) {
            cerr << "a valid internet connection is required" << endl;
            exit(EXIT_FAILURE);
        }
    }
};

BOOST_GLOBAL_FIXTURE(http_fixture);
