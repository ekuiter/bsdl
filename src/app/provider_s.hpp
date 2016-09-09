#pragma once

#include "provider.hpp"
#include "settings.hpp"
#include <iostream>
#include <regex>
#include <thread>
#include <Node.h>

#ifdef __MINGW32__
#include <windows.h>
#endif

using namespace std;
using namespace http;

class provider::s : public provider {
    s(): provider(settings::get("provider_s")), max_retries(4), cooldown(12) {}

    const int max_retries;
    const chrono::seconds cooldown;

    request::download try_to_fetch(const request& _request) const {
        string script, id = _request.get_path().substr(1, _request.get_path().substr(1).find("/"));
        unique_ptr<CDocument> document = request(_request).set_method(request::method::POST).
                set_fields({{ {"op", settings::get("provider_s_op")}, {"id", id} }})().parse();

        try {
            script = document->find(settings::get("provider_s_player_sel")).assertNum(3).nodeAt(2).text();
        } catch (runtime_error) {
            throw retry();
        }

        regex file_pattern(settings::get("provider_s_file_pattern"));
        smatch results;
        if (!regex_search(script, results, file_pattern))
            throw not_found(_request);
        return request::download(results[1]);
    }

    class retry : public std::exception {};

public:
    static s& instance() {
        static s instance;
        return instance;
    }

    string get_file_format() const override {
        return settings::get("provider_s_file_format");
    }

    request::download fetch(const request& _request) const override {
        for (int i = 0; i < max_retries; i++) {
            try {
                return try_to_fetch(_request);
            } catch (retry) {
                cerr << "could not fetch file, retrying in " << cooldown.count() << " seconds ...";
#ifdef __MINGW32__
                Sleep(chrono::duration_cast<chrono::milliseconds>(cooldown).count());
#else
                this_thread::sleep_for(cooldown);
#endif
                cerr << " retry." << endl;
            }
        }
        throw not_found(_request);
    }
};
