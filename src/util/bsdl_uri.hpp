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
        string uri, search_string, series_url, bs_series_url;

        void filter_search_results(vector<aggregators::series*>& search_results, const string& url) const {
            auto predicate = [&url](aggregators::series* series) {
                return series->get_request().get_url() != url;
            };
            search_results.erase(remove_if(search_results.begin(), search_results.end(), predicate), search_results.end());
        }

        vector<aggregators::series*> fetch_and_filter_series(aggregators::aggregator& aggregator,
                                                             const string& filter_url, const string& series_type) const {
            vector<aggregators::series*> search_results = aggregator.search_single(search_string);
            if (filter_url != "")
                filter_search_results(search_results, filter_url);
            if (search_results.size() == 0)
                throw uri_error(string("no ") + series_type + " at URI " + uri);
            return util::check_unambiguous(string("ambiguous ") + series_type + " at URI " + uri + ": ", search_results);
        }

        aggregators::bs::series* fetch_bs_series(aggregators::bs::mergeable_series* mergeable_series) const {
            auto empty_series = new aggregators::bs::series(aggregators::bs::bs::instance(), aggregators::bs::empty_series_title());
            
            if (bs_series_url == "")
                return empty_series;
            else {
                auto search_results = fetch_and_filter_series(aggregators::bs::bs::instance(), bs_series_url, "bs series");
                aggregators::series* bs_series = search_results[0];
                bs_series->load();
                return dynamic_cast<aggregators::bs::series*>(bs_series);
            }
        }
        
    public:
        bsdl_uri(const aggregators::series& series):
            aggregator(const_cast<aggregators::aggregator*>(&series.get_aggregator())), search_string(app::instance().get_series_search()) {
            uri = string("bsdl://") + aggregator->get_name() +
                "/" + encode_uri(search_string) +
                "/" + encode_uri(series.get_request().get_url());
            auto mergeable_series = dynamic_cast<aggregators::bs::mergeable_series*>(const_cast<aggregators::series*>(&series));
            if (mergeable_series && mergeable_series->get_bs_series() &&
                mergeable_series->get_bs_series()->get_title() != aggregators::bs::empty_series_title())
                uri += "/" + encode_uri(mergeable_series->get_bs_series()->get_request().get_url());
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
            string path = results[3];
            boost::trim_left_if(path, boost::is_any_of("/"));
            vector<string> path_parts;
            boost::split(path_parts, path, boost::is_any_of("/"));
            search_string = decode_uri(path_parts[0]);
            if (search_string == "")
                throw uri_error(string("URI ") + uri + " doesn't have a series title");
            if (path_parts.size() > 1)
                series_url = decode_uri(path_parts[1]);
            if (path_parts.size() > 2)
                bs_series_url = decode_uri(path_parts[2]);
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
            auto search_results = fetch_and_filter_series(*aggregator, series_url, "series");
            aggregators::series* series = search_results[0];
            auto mergeable_series = dynamic_cast<aggregators::bs::mergeable_series*>(series);
            if (mergeable_series)
                mergeable_series->set_bs_series(fetch_bs_series(mergeable_series));
            else if (bs_series_url != "")
                throw uri_error(string("no mergeable series at URI ") + uri);;
            return *series;
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
