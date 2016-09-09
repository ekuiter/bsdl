#pragma once

#include "provider.hpp"
#include "settings.hpp"
#include <iostream>
#include <Node.h>

using namespace std;
using namespace http;

class provider::v : public provider {
    v(): provider(settings::get("provider_v")) {}

public:
    static v& instance() {
        static v instance;
        return instance;
    }

    string get_file_format() const override {
        return settings::get("provider_v_file_format");
    }

    request::download fetch(const request& _request) const override {
        request https_request = request(_request).set_https(true);
        unique_ptr<CDocument> document = https_request().parse();
        string hash = document->find(settings::get("provider_v_hash_sel")).assertNum(1).nodeAt(0).attribute("value"),
                timestamp = document->find(settings::get("provider_v_timestamp_sel")).assertNum(1).nodeAt(0).attribute("value");

        document = https_request.set_method(request::method::POST).
                set_fields({{ {"hash", hash}, {"timestamp", timestamp}, {"throttle", "0"} }})().parse();

        return request::download(document->find(settings::get("provider_v_player_sel")).assertNum(1).nodeAt(0).attribute("data-url"));
    }
};
