#pragma once

#include <fstream>
#include <boost/test/unit_test.hpp>
#include <json.hpp>
#include "../settings.hpp"
#include "../http/request.hpp"
#include "../providers/provider.hpp"
#include "../providers/provider_youtube_dl.hpp"
#include "../aggregators/aggregator.hpp"

#define MOCK_THROW(return_type, fn, signature)  \
    return_type fn signature override {         \
        throw mock_error(#fn);                  \
    }

#define MOCK_EMPTY(return_type, fn, signature)  \
    return_type fn signature override {}

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

string executable_file();

struct settings_fixture {
    settings_fixture(vector<string> args = {}) {
        args.insert(args.begin(), executable_file());
        settings::instance().read(args);
    }

    ~settings_fixture() {
        settings::instance().reset_instance();
    }
};

struct data_fixture {
    nlohmann::json data;

    data_fixture() {
        string data_file_name = "data.json";
        ifstream data_file(settings::instance().resource_file({ executable_file() }, data_file_name));
        BOOST_REQUIRE_MESSAGE(data_file.good(), data_file_name + " can not be read");
        data_file >> data;
        for (auto key : {"some_aggregator", "another_aggregator", "some_provider", "another_provider",
                    "some_subtitle", "another_subtitle", "youtube_dl_provider", "youtube_dl_request"})
            BOOST_REQUIRE_MESSAGE(data.count(key) == 1, string(key) + " is not set in " + data_file_name);
    }

    aggregators::aggregator& some_aggregator() { return aggregators::aggregator::instance(data["some_aggregator"]); }
    aggregators::aggregator& another_aggregator() { return aggregators::aggregator::instance(data["another_aggregator"]); }
    providers::provider& some_provider() { return providers::provider::instance(data["some_provider"], true); }
    providers::provider& another_provider() { return providers::provider::instance(data["another_provider"], true); }
    providers::provider& youtube_dl_provider() { return providers::provider::instance(data["youtube_dl_provider"], true); }
    http::request youtube_dl_request() { return http::request(data["youtube_dl_request"].get<string>()); }
    aggregators::subtitle& some_subtitle() { return aggregators::subtitle::instance(data["some_subtitle"]); }
    aggregators::subtitle& another_subtitle() { return aggregators::subtitle::instance(data["another_subtitle"]); }
};

template<class First, class... Rest>
struct multi_fixture : public First, public multi_fixture<Rest...> {};

template<class First>
struct multi_fixture<First> : public First {};

typedef multi_fixture<settings_fixture, data_fixture> settings_and_data_fixture;
