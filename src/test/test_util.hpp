#pragma once

#include <boost/test/unit_test.hpp>
#include "fixtures.hpp"

#define MOCK_THROW(return_type, fn, signature)  \
    return_type fn signature override {         \
        throw mock_error(#fn);                  \
    }

#define MOCK_EMPTY(return_type, fn, signature)  \
    return_type fn signature override {}

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

string executable_file();
string resource_file(const string& resource);
