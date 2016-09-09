#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include "request.hpp"
#include "response.hpp"
#include <curl_easy.h>

using namespace std;

namespace http {

    class request::implementation_base {
    public:
        virtual curl::curl_easy& get_curl() = 0;
        virtual void perform(bool multi = false) = 0;
        virtual void remove_file() = 0;
    };

    class request::implementation : public implementation_base {
        const request& _request;
        stringstream stream;
        curl::curl_ios<stringstream> writer;
        curl::curl_easy curl;
        bool performed, response_read;

    public:
        implementation(const request& request);

        curl::curl_easy& get_curl() {
            return curl;
        }

        void perform(bool multi = false);
        void remove_file();
        response read_response();
    };

    class request::download::implementation : public implementation_base {
        const request::download& download_request;
        string file_name;
        ofstream file;
        curl::curl_ios<ostream> writer;
        curl::curl_easy curl;
        bool performed, success;

    public:
        implementation(const request::download& _download_request, const string& file_name);

        curl::curl_easy& get_curl() {
            return curl;
        }

        void perform(bool multi = false);
        void remove_file();
        bool get_success();
    };
}