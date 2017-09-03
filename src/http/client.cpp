#include "client.hpp"
#include "exception.hpp"
#include "request_implementation.hpp"
#include <sstream>
#include <fstream>
#include <curl_header.h>

using namespace curl;

namespace http {
    void client::prepare(const request& _request, request::implementation_base& implementation) const {
        curl_easy& curl = implementation.get_curl();

        curl.add<CURLOPT_URL>(_request.get_url().c_str());
        curl.add<CURLOPT_FOLLOWLOCATION>(true);
        curl.add<CURLOPT_NOPROGRESS>(false);
        curl.add<CURLOPT_VERBOSE>(verbose);

        curl.add<CURLOPT_XFERINFODATA>((void*) &_request);
        curl.add<CURLOPT_XFERINFOFUNCTION>([](void* payload, curl_off_t dltotal, curl_off_t dlnow, curl_off_t, curl_off_t) {
            const request* _request = (const request*) payload;
            return _request->get_callback()(request::status::RUNNING, *_request, dlnow, dltotal, nullptr);
        });

        if (_request.is_fetch()) {
            curl.add<CURLOPT_HEADER>(true);
            curl.add<CURLOPT_TIMEOUT>(timeout);
        }

        if (_request.get_method() == request::method::POST)
            curl.add<CURLOPT_COPYPOSTFIELDS>(static_cast<string>(_request.get_fields()).c_str());
    }

    bool client::perform(const request& _request) const {
        curl_easy& curl = _request.get_implementation().get_curl();
        request::callback _callback = _request.get_callback();

        curl_header header;
        for (auto& header_pair : _request.get_headers())
            header.add(header_pair.first + ": " + header_pair.second);
        curl.add<CURLOPT_HTTPHEADER>(header.get());

        try {
            _callback(request::status::BEGIN, _request, 0, 0, nullptr);
            curl.perform();
            _callback(request::status::END, _request, 0, 0, nullptr);
            return true;
        } catch (curl_easy_exception e) {
            bool aborted = e.get_traceback()[0] ==
                    pair<string, string>("Operation was aborted by an application callback", "perform");
            return _callback(aborted ? request::status::END | request::status::ABORT : request::status::END, _request, 0, 0, &e);
        }
    }
}
