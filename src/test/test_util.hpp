#pragma once

#include <boost/test/unit_test.hpp>
#include "fixtures.hpp"

#undef timeout

#define MOCK_THROW(return_type, fn, signature)  \
    return_type fn signature override {         \
        throw mock_error(#fn);                  \
    }

#define MOCK_EMPTY(return_type, fn, signature)  \
    return_type fn signature override {}

#define LONG_RUNNING_TEST_SUITE(type, ...)                              \
    BOOST_##type##_TEST_SUITE(long_running, __VA_ARGS__ * utf::label("long_running"))

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

string executable_file();
string resource_file(const string& resource);
default_random_engine& random_engine();
