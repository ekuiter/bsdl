#pragma once

#include <fstream>
#include <random>
#include <boost/test/unit_test.hpp>
#include <json.hpp>
#include "../settings.hpp"
#include "../http/request.hpp"
#include "../providers/provider.hpp"
#include "../providers/provider_youtube_dl.hpp"
#include "../aggregators/aggregator.hpp"

string executable_file();
string resource_file(const string& resource);
default_random_engine& random_engine();

// settings fixture

struct settings_fixture {
    settings_fixture(vector<string> args = {}) {
        args.insert(args.begin(), executable_file());
        reset_settings();
        settings::instance().read(args);
    }

    ~settings_fixture() {
        reset_settings();
    }

    void reset_settings() {
        settings::instance().reset_instance();
    }
};

// data fixture

#define SOME_SERIES(name)                                       \
    string name##_series() {                                    \
        vector<string> series = data[#name "_series"];          \
        shuffle(series.begin(), series.end(), random_engine()); \
        string series_title = *series.begin();                  \
        boost::to_lower(series_title);                          \
        return series_title;                                    \
    }

struct data_fixture {
    nlohmann::json data;

    data_fixture() {
        string data_file_name = "data.json";
        ifstream data_file(resource_file(data_file_name));
        BOOST_REQUIRE_MESSAGE(data_file.good(), data_file_name + " can not be read");
        data_file >> data;
        for (auto key : {"some_aggregator", "another_aggregator", "some_provider", "another_provider",
                    "some_subtitle", "another_subtitle", "youtube_dl_provider", "youtube_dl_request"})
            BOOST_REQUIRE_MESSAGE(data.count(key) == 1, string(key) + " is not set in " + data_file_name);
    }

    string get_data(const string& key) { return data[key].get<string>(); }
    aggregators::aggregator& some_aggregator() { return aggregators::aggregator::instance(get_data("some_aggregator")); }
    aggregators::aggregator& another_aggregator() { return aggregators::aggregator::instance(get_data("another_aggregator")); }
    providers::provider& some_provider() { return providers::provider::instance(get_data("some_provider"), true); }
    providers::provider& another_provider() { return providers::provider::instance(get_data("another_provider"), true); }
    providers::provider& youtube_dl_provider() { return providers::provider::instance(get_data("youtube_dl_provider"), true); }
    http::request youtube_dl_request() { return http::request(get_data("youtube_dl_request")); }
    aggregators::subtitle& some_subtitle() { return aggregators::subtitle::instance(get_data("some_subtitle")); }
    aggregators::subtitle& another_subtitle() { return aggregators::subtitle::instance(get_data("another_subtitle")); }

    SOME_SERIES(bs)
    SOME_SERIES(kx)
    SOME_SERIES(mk)
    SOME_SERIES(pr)
};

// silent stream fixture

struct mockbuf : public streambuf {
    virtual int overflow(int c) override {
        return c;
    }
};
    
#define SILENT_STREAM_FIXTURE(s)                \
    class silent_##s##_fixture {                \
        ostream& stream = s;                    \
        mockbuf dstbuf;                         \
        streambuf* srcbuf;                      \
                                                \
    public:                                     \
    silent_##s##_fixture() {                    \
        srcbuf = stream.rdbuf();                \
        stream.rdbuf(&dstbuf);                  \
    }                                           \
                                                \
    ~silent_##s##_fixture() {                   \
        stream.rdbuf(srcbuf);                   \
    }                                           \
    }

SILENT_STREAM_FIXTURE(cout);
SILENT_STREAM_FIXTURE(cerr);
SILENT_STREAM_FIXTURE(clog);

// multi fixture

template<class First, class... Rest>
struct multi_fixture : public First, public multi_fixture<Rest...> {};

template<class First>
struct multi_fixture<First> : public First {};

typedef multi_fixture<settings_fixture, data_fixture> settings_and_data_fixture;
