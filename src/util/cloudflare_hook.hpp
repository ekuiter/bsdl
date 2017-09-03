#pragma once

#include <iostream>
#include "../http/client.hpp"

using namespace std;

namespace util {    
    class cloudflare_hook : public http::request::hook {
        bool is_running_hook = false;
        map<string, string> clearances;

        cloudflare_hook() {}

        string& get_clearance(http::request& request) {
            return clearances[request.get_host()];
        }

        bool has_clearance(http::request& request) {
            return get_clearance(request) != "";
        }

        string user_agent();
        bool has_cloudflare(const http::response& response);
        string fetch_clearance(http::request& request);
        void modify_request(http::request& request);
        
    public:
        static cloudflare_hook& instance() {
            static cloudflare_hook hook;
            return hook;
        }

        void operator()(http::request& request) override;
        http::response operator()(http::request request, http::response response) override;
    };
}
