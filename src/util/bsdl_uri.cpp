#include <iostream>
#include "bsdl_uri.hpp"
#include "../app.hpp"

using namespace std;

namespace util {
    vector<aggregators::series*> check_unambiguous(string msg, vector<aggregators::series*> search_results) {
        if (search_results.size() > 1) {
            for (int i = 0, n = search_results.size(); i < n; i++)
                msg += search_results[i]->get_title() + (i == n - 1 ? "" : ", ");
            throw uri_error(msg);
        }
        return search_results;
    }
    
    vector<string> bsdl_uri::split(string str, const string& delimiter) {
        vector<string> parts;
        boost::trim_left_if(str, boost::is_any_of(delimiter));
        boost::split(parts, str, boost::is_any_of(delimiter));
        return parts;
    }

    void bsdl_uri::parse_host(const string& host) {
        try {
            aggregator = &aggregators::aggregator::instance(host);
        } catch (aggregators::exception& e) {
            throw uri_error(*this, "has invalid aggregator " + host);
        }
    }
        
    void bsdl_uri::parse_path(const string& path) {
        vector<string> path_parts = split(path, "/");
        search_string = decode_uri(path_parts[0]);
        if (search_string == "")
            throw uri_error(*this, "doesn't have a series title");
        if (path_parts.size() > 1)
            series_url = decode_uri(path_parts[1]);
        if (path_parts.size() > 2)
            bs_series_url = decode_uri(path_parts[2]);
        if (path_parts.size() > 3)
            throw uri_error(*this, "has invalid path");
    }

    void bsdl_uri::parse_query(const string& query) {
        static vector<string> allowed_keys = { "providers", "subtitles" };
        for (auto pair : split(query, "&")) {
            vector<string> pair_parts = split(pair, "=");
            if (pair_parts.size() != 2 || pair_parts[0] == "" || pair_parts[1] == "")
                throw uri_error(*this, "has invalid key-value pair '" + pair + "'");
            string key = decode_uri(pair_parts[0]), value = decode_uri(pair_parts[1]);
            if (find(allowed_keys.begin(), allowed_keys.end(), key) == allowed_keys.end())
                throw uri_error(*this, "has invalid key '" + key + "'");
            params[key] = value;
        }
    }

    void bsdl_uri::process_params() {
        settings& settings = settings::instance();
        if (params["providers"] != "") {
            settings["providers"] = params["providers"];
            providers::provider::set_preferred_providers(settings.update_preferred_providers(true));
        }
        if (params["subtitles"] != "") {
            settings["subtitles"] = params["subtitles"];
            aggregators::subtitle::set_preferred_subtitles(settings.update_preferred_subtitles(true));
        }
    }

    void bsdl_uri::filter_search_results(vector<aggregators::series*>& search_results, const string& url) const {
        auto predicate = [&url](aggregators::series* series) {
            return series->get_request().get_url() != url;
        };
        search_results.erase(remove_if(search_results.begin(), search_results.end(), predicate), search_results.end());
    }

    vector<aggregators::series*> bsdl_uri::fetch_and_filter_series(aggregators::aggregator& aggregator,
                                                         const string& filter_url, const string& series_type) const {
        vector<aggregators::series*> search_results = aggregator.search_single(search_string);
        if (filter_url != "")
            filter_search_results(search_results, filter_url);
        if (search_results.size() == 0)
            throw uri_error(string("no ") + series_type + " at URI " + uri);
        return util::check_unambiguous(string("ambiguous ") + series_type + " at URI " + uri + ": ", search_results);
    }

    aggregators::bs::series* bsdl_uri::fetch_bs_series(aggregators::bs::mergeable_series* mergeable_series) const {
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

    bsdl_uri::bsdl_uri(const aggregators::series& series):
        aggregator(const_cast<aggregators::aggregator*>(&series.get_aggregator())), search_string(app::instance().get_series_search()) {
        uri = string("bsdl://") + aggregator->get_name() +
            "/" + encode_uri(search_string) +
            "/" + encode_uri(series.get_request().get_url());
        auto mergeable_series = dynamic_cast<aggregators::bs::mergeable_series*>(const_cast<aggregators::series*>(&series));
        if (mergeable_series && mergeable_series->get_bs_series() &&
            mergeable_series->get_bs_series()->get_title() != aggregators::bs::empty_series_title())
            uri += "/" + encode_uri(mergeable_series->get_bs_series()->get_request().get_url());
    }
        
    bsdl_uri::bsdl_uri(const string& _uri): uri(_uri) {
        // see stackoverflow.com/q/2616011
        regex uri_pattern("bsdl://([^/ :]+):?([^/ ]*)(/?[^ #?]*)\\x3f?([^ #]*)#?([^ ]*)");
        smatch results;
        if(!regex_search(uri, results, uri_pattern))
            throw uri_error(*this, "is invalid");
        string host = results[1], port = results[2], path = results[3],
            query = results[4], fragment = results[5];
        parse_host(host);
        parse_path(path);
        if (query != "")
            parse_query(query);
        process_params();
    }

    aggregators::series& bsdl_uri::fetch_series() const {
        auto search_results = fetch_and_filter_series(*aggregator, series_url, "series");
        aggregators::series* series = search_results[0];
        auto mergeable_series = dynamic_cast<aggregators::bs::mergeable_series*>(series);
        if (mergeable_series)
            mergeable_series->set_bs_series(fetch_bs_series(mergeable_series));
        else if (bs_series_url != "")
            throw uri_error(string("no mergeable series at URI ") + uri);
        return *series;
    }
}
