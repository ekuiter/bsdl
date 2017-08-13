#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "request.hpp"
#include "request_implementation.hpp"
#include "exception.hpp"

#undef timeout
#include <curl_multi.h>

using namespace std;

namespace http {
    namespace multi {
        template <typename T1>
        class base {
        protected:
            curl::curl_multi curl;
            vector<T1*> requests;

            base() {}

            void _add(T1& request) {
                request.log();
                requests.push_back(&request);
                curl.add(request.create_implementation().get_curl());
            }

            void _remove(T1& request) {
                _remove(find(requests.begin(), requests.end(), &request));
            }

            void _remove(typename vector<T1*>::iterator it) {
                if (it == requests.end())
                    throw exception("request is not in the multi request");
                curl.remove((*it)->get_implementation().get_curl());
                (*it)->destroy_implementation();
                requests.erase(it);
            }

        public:
            int operator()() {
                curl.wait(nullptr, 0, 1000, nullptr);
                curl.perform();

                int _;
                CURLMsg* message = nullptr;
                while ((message = curl_multi_info_read(curl.get_curl(), &_))) {
                    if (message->data.result != CURLE_OK && message->data.result != CURLE_ABORTED_BY_CALLBACK)
                        cerr << "curl error " << message->data.result << endl;
                    for (auto it = requests.begin(); it != requests.end(); it++)
                        if ((*it)->get_implementation().get_curl().get_curl() == message->easy_handle) {
                            _remove(it);
                            break;
                        }
                }

                return curl.get_active_transfers();
            }
        };

        class request : public base<http::request> {
            typedef unordered_map<const http::request*, response> results_map;
            results_map results;

            void process_result(http::request& _request, results_map& results) {
                _request.get_implementation().perform(true);
                response _response =
                        dynamic_cast<http::request::implementation&>(_request.get_implementation()).read_response();
                results.insert({&_request, _response});
            }

        public:
            request() {}

            void add(http::request& request) {
                if (!request.is_fetch())
                    throw exception("request has to be fetch request");
                _add(request);
            }

            void remove(http::request& request) {
                if (!request.is_fetch())
                    throw exception("request has to be fetch request");
                _remove(request);
            }

            const results_map& get_results() {
                for (auto& request : requests)
                    process_result(*request, results);

                for (auto& request : requests) {
                    curl.remove(request->get_implementation().get_curl());
                    request->destroy_implementation();
                }

                return results;
            }

            class download;
        };

        class request::download : public base<http::request::download> {
        public:
            download() {}

            void add(http::request::download& request) {
                _add(request);
            }

            void remove(http::request::download& request) {
                _remove(request);
            }
        };
    }
}
