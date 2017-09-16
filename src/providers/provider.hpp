#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include "../http/client.hpp"

using namespace std;

namespace providers {
    class provider;

    ostream& operator<<(ostream& stream, const provider& provider);

    class provider {
        string name;
        static vector<provider*> preferred_providers;
        static vector<string> get_provider_names(const string& setting);
        static bool youtube_dl_installed;
        static bool youtube_dl_updated;

        provider(const string& _name): name(_name) {}

    public:
        static const int count;
        static provider& instance(const string& name, bool should_throw = false);
        static string get_provider_name(const string& setting);
        static bool is_provider(const string& name, const string& setting);
        static void set_preferred_providers(const vector<provider*>& _preferred_providers);

        static const vector<provider*>& get_preferred_providers() {
            return preferred_providers;
        }

        const string& get_name() const {
            return name;
        }

        virtual string get_file_format() const = 0;
        virtual http::request::download fetch(const http::request& request) const = 0;

        class exception : public runtime_error {
        public:
            exception(const string& msg): runtime_error(msg) {}
        };

        class not_found : public exception {
        public:
            not_found(const http::request& request): exception(string("no video file found at ") + request.get_url()) {}
        };

        class unknown;
        
        template <int Id>
        class youtube_dl;
    };

    class provider::unknown : public provider {
        mutable bool error_logged;

        unknown(const string& _name): provider(_name), error_logged(false) {}

    public:
        static unknown& instance(const string& name) {
            static unordered_map<string, unknown> unknown_providers;
            return unknown_providers.insert({name, unknown(name)}).first->second;
        }

        void log_error() const {
            if (get_name() != "Unavailable" && !error_logged) {
                cerr << "encountered unknown provider \"" << get_name() << "\"" << endl;
                error_logged = true;
            }
        }

        string get_file_format() const override {
            log_error();
            return "unknown";
        }

        http::request::download fetch(const http::request&) const override {
            log_error();
            return http::request::download();
        }
    };
}
