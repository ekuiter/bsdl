#pragma once

#include <iostream>
#include <cassert>
#include <unordered_map>
#include <memory>
#include "headers.hpp"
#include "fields.hpp"
#include <curl_easy.h>

using namespace std;

namespace http {
    class response;

    class request {
    public:
        enum class method { GET, POST };
        enum class status { BEGIN = 0b1, RUNNING = 0b10, END = 0b100, ABORT = 0b1000 };
        typedef function<int (status, const request&, curl_off_t, curl_off_t, curl::curl_easy_exception*)> callback;

        class implementation_base;
        class implementation;
        class download;

    protected:
        string host, path;
        bool https;
        method _method;
        headers _headers;
        fields _fields;
        callback _callback;
        mutable shared_ptr<implementation> _implementation;

    public:
        request(): https(false) {}
        request(const string& _host, const string& _path, bool _https = false, method method = method::GET,
                const headers& headers = {}, const fields& fields = {}):
                host(_host), path(_path), https(_https), _method(method), _headers(headers), _fields(fields) {}
        request(const string& url, method method = method::GET, const headers& headers = {}, const fields& fields = {});

        bool is_empty() const noexcept {
            return host == "";
        }

        const string& get_host() const noexcept {
            return host;
        }

        request& set_host(const string& _host) noexcept {
            host = _host;
            return *this;
        }

        const string& get_path() const noexcept {
            return path;
        }

        request& set_path(const string& _path) noexcept {
            path = _path;
            return *this;
        }

        const bool get_https() const noexcept {
            return https;
        }

        request& set_https(bool _https) noexcept {
            https = _https;
            return *this;
        }

        const method get_method() const noexcept {
            return _method;
        }

        const string get_method_string() const noexcept {
            return _method == method::GET ? "GET" : "POST";
        }

        request& set_method(method method) noexcept {
            _method = method;
            return *this;
        }

        const headers& get_headers() const noexcept {
            return _headers;
        }

        headers& get_headers() noexcept {
            return _headers;
        }

        request& set_headers(const headers& headers) noexcept {
            _headers = headers;
            return *this;
        }

        const fields& get_fields() const noexcept {
            return _fields;
        }

        fields& get_fields() noexcept {
            return _fields;
        }

        request& set_fields(const fields& fields) noexcept {
            _fields = fields;
            return *this;
        }

        callback get_callback() const;

        request& set_callback(callback callback) noexcept {
            _callback = callback;
            return *this;
        }

        virtual request::implementation_base& create_implementation() const;
        virtual void destroy_implementation() const;
        virtual implementation_base& get_implementation() const noexcept;

        const string get_url() const noexcept {
            return string("http") + (https ? "s" : "") + "://" + host + path;
        }

        const request& log() const {
            clog << "=> " << get_method_string() << " " << get_url() << endl;
            if (get_headers().size())
                clog << "   Headers: " << get_headers() << endl;
            if (get_fields().size())
                clog << "   Fields: " << get_fields() << endl;
            return *this;
        }

        virtual bool is_fetch() const noexcept {
            return true;
        }

        virtual bool is_download() const noexcept {
            return false;
        }

        virtual response operator()() const;
        virtual bool operator()(const string& file_name) const;
        request get_relative(const string& path) const;
    };

    class request::download : public request {
        class implementation;
        mutable string file_name;
        mutable shared_ptr<implementation> download_implementation;

    public:
        download() {}
        download(const string& _host, const string& _path, bool _https = false, method method = method::GET,
                 const headers& headers = {}, const fields& fields = {}):
                request(_host, _path, _https, method, headers, fields) {}
        download(const string& url, method method = method::GET, const headers& headers = {}, const fields& fields = {}):
                request(url, method, headers, fields) {}

        download& set_file_name(const string& _file_name) {
            file_name = _file_name;
            return *this;
        }

        request::implementation_base& create_implementation() const;
        void destroy_implementation() const;
        implementation_base& get_implementation() const noexcept;

        bool is_fetch() const noexcept {
            return false;
        }

        bool is_download() const noexcept {
            return true;
        }

        response operator()() const;
        bool operator()(const string& file_name) const;
        void remove_file() const;
    };

    ostream& operator<<(ostream& stream, const request& request);

    inline long operator&(request::status lhs, request::status rhs) {
        return static_cast<long>(lhs) & static_cast<long>(rhs);
    }

    inline request::status operator|(request::status lhs, request::status rhs) {
        return (request::status) (static_cast<long>(lhs) | static_cast<long>(rhs));
    }

    inline request::status operator|=(request::status& lhs, request::status rhs) {
        return lhs = (request::status) (static_cast<long>(lhs) | static_cast<long>(rhs));
    }
}