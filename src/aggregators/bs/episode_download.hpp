#pragma once

#include <iostream>
#include <functional>
#include "episode.hpp"
#include "episode_file.hpp"
#include "../../http/client.hpp"

using namespace std;

namespace aggregators {
    namespace bs {
        class episode::download {
        public:
            typedef function<void (download&)> refresh_callback;

        private:
            const episode& _episode;
            int now_bytes, total_bytes;
            http::request::download download_request;
            bool loaded, abort;
            string message;
            refresh_callback _refresh_callback;
            file _file;

        public:
            download(const episode& episode, refresh_callback refresh_callback = [](download&) {}):
                    _episode(episode), now_bytes(0), total_bytes(0), loaded(false), abort(false), message("Queued"),
                    _refresh_callback(refresh_callback), _file(episode) {}

            const episode& get_episode() const {
                return _episode;
            }

            int get_now_bytes() const {
                return now_bytes;
            }

            int get_total_bytes() const {
                return total_bytes;
            }

            http::request::download& get_download_request();
            string get_file_name() const;
            string get_file_path(string directory_name) const;

            const http::request::download* get_download_request_pointer() const {
                return loaded ? &download_request : nullptr;
            }

            const string& get_message() const {
                return message;
            }

            void set_message(const string& _message) {
                message = _message;
                _refresh_callback(*this);
            }

            void set_abort(bool _abort) {
                if ((abort = _abort))
                    set_message("Aborted");
            }

            bool operator==(const download& other) {
                return get_file_name() == other.get_file_name(); // suffices for our use
            }

            int http_callback(http::request::status status, const http::request& request,
                              curl_off_t _now_bytes, curl_off_t _total_bytes, curl::curl_easy_exception* e);
        };

        ostream& operator<<(ostream& stream, const episode::download& episode_download);
    }
}
