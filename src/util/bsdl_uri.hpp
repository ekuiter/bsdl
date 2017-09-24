#pragma once

#include <iostream>
#include <regex>
#include <unordered_map>
#include "../settings.hpp"
#include "../aggregators/aggregator.hpp"
#include "../aggregators/bs/bs.hpp"
#include "../aggregators/bs/merge_transform.hpp"

namespace util {
    string encode_uri(const string &input);
    string decode_uri(const string &input);
    vector<aggregators::series*> check_unambiguous(string msg, vector<aggregators::series*> search_results);
    
    class bsdl_uri {
    private:
        aggregators::aggregator* aggregator = nullptr;
        string uri, search_string, series_url, bs_series_url;
        unordered_map<string, string> params;

        vector<string> split(string str, const string& delimiter);
        void parse_host(const string& host);
        void parse_path(const string& path);
        void parse_query(const string& query);
        void process_params();
        void process_special(const string& fragment);
        void filter_search_results(vector<aggregators::series*>& search_results, const string& url) const;
        vector<aggregators::series*> fetch_and_filter_series(aggregators::aggregator& aggregator,
                                                             const string& filter_url, const string& series_type) const;
        aggregators::bs::series* fetch_bs_series(aggregators::bs::mergeable_series* mergeable_series) const;
        
    public:
        bsdl_uri(const aggregators::series& series);
        bsdl_uri(const string& _uri, bool only_special = false);
        aggregators::series& fetch_series() const;
        
        const string& get_uri() const {
            return uri;
        }
        
        const string& get_search_string() const {
            return search_string;
        }        
    };

    class uri_error : public runtime_error {
    public:
        uri_error(const string& msg): runtime_error(msg) {}
        uri_error(const bsdl_uri& uri, const string& msg): runtime_error(string("URI ") + uri.get_uri() + " " + msg) {}
    };

    class search_query {
        string search_string;
        unique_ptr<bsdl_uri> uri;
        
    public:
        search_query() {}
        
        search_query(const string& _search_string): search_string(_search_string) {
            boost::trim(search_string);
            if (search_string.find("bsdl://") == 0)
                uri.reset(new bsdl_uri(search_string));
        }

        bool is_empty() const {
            return search_string == "";
        }

        string get_search_string() const {
            if (is_empty())
                throw runtime_error("empty search query");
            if (uri)
                return uri->get_search_string();
            else
                return search_string;
        }

        vector<aggregators::series*> fetch_results() const {
            if (is_empty())
                throw runtime_error("empty search query");
            if (uri)
                return { &uri->fetch_series() };
            return aggregators::aggregator::search(search_string);
        }

        vector<aggregators::series*> check_unambiguous(vector<aggregators::series*> search_results) const {
            return util::check_unambiguous(string("ambiguous series for ") + search_string + ": ", search_results);
        }
    };
}
