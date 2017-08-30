#pragma once

#include <iostream>
#include <regex>
#include "../aggregators/aggregator.hpp"

namespace util {
    string encode_uri(const string &input);
    string decode_uri(const string &input);
    
    namespace {
        inline vector<aggregators::series*> check_unambiguous(string msg, vector<aggregators::series*> search_results) {
            if (search_results.size() > 1) {
                for (int i = 0, n = search_results.size(); i < n; i++)
                    msg += search_results[i]->get_title() + (i == n - 1 ? "" : ", ");
                throw runtime_error(msg);
            }
            return search_results;
        }
    }
    
    class uri_error : public runtime_error {
    public:
        uri_error(const string& msg): runtime_error(msg) {}
    };
    
    class bsdl_uri {
        aggregators::aggregator* aggregator = nullptr;
        string uri, search_string, request_url;
        
    public:
        bsdl_uri(const aggregators::series& series):
            aggregator(const_cast<aggregators::aggregator*>(&series.get_aggregator())), search_string(app::instance().get_series_search()) {
            uri = string("bsdl://") + aggregator->get_name() +
                "/" + encode_uri(search_string) +
                "?" + encode_uri(series.get_request().get_url());
        }
        
        bsdl_uri(const string& _uri): uri(_uri) {
            // see stackoverflow.com/q/2616011
            regex uri_pattern("bsdl://([^/ :]+):?([^/ ]*)(/?[^ #?]*)\\x3f?([^ #]*)#?([^ ]*)");
            smatch results;
            if(!regex_search(uri, results, uri_pattern))
                throw uri_error(string("URI ") + uri + " is invalid");
            try {
                aggregator = &aggregators::aggregator::instance(results[1]);
            } catch (aggregators::exception& e) {
                throw uri_error(string("URI ") + uri + " has invalid aggregator " + (string) results[1]);
            }
            search_string = results[3];
            boost::trim_left_if(search_string, boost::is_any_of("/"));
            search_string = decode_uri(search_string);
            if (search_string == "")
                throw uri_error(string("URI ") + uri + " doesn't have a series title");
            request_url = decode_uri(results[4]);
        }

        aggregators::aggregator& get_aggregator() const {
            return *aggregator;
        }

        const string& get_uri() const {
            return uri;
        }
        
        const string& get_search_string() const {
            return search_string;
        }

        aggregators::series& fetch_series() const {
            vector<aggregators::series*> search_results = aggregator->search_single(search_string);
            if (request_url != "") {
                auto predicate = [this](aggregators::series* series) {
                    return series->get_request().get_url() != request_url;
                };
                search_results.erase(remove_if(search_results.begin(), search_results.end(), predicate), search_results.end());
            }
            if (search_results.size() == 0)
                throw uri_error(string("no series at URI ") + uri);
            util::check_unambiguous(string("ambiguous series at URI ") + uri + ": ", search_results);
            return *search_results[0];
        }
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
