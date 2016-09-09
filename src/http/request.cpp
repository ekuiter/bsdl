#include "request.hpp"
#include "request_implementation.hpp"
#include "client.hpp"
#include "exception.hpp"
#include <regex>

namespace http {
    request::request(const string& url, method method, const headers& headers, const fields& fields):
            _method(method), _headers(headers), _fields(fields) {
        regex url_pattern("http(s?)://(.+?)($|(?:/.*))"), fragment_pattern("(.*?)(?:$|#)");
        smatch results;
        if (!regex_search(url, results, url_pattern))
            throw exception((url == "" ? "(empty url)" : url) + " is an invalid url");
        host = results[2], path = results[3], https = results[1] == "s";
        if (path == "")
            path = "/";
        regex_search(path, results, fragment_pattern);
        path = results[1];
    }

    request::callback request::get_callback() const {
        return _callback ? _callback : client::instance().get_callback();
    }

    request::implementation_base& request::create_implementation() const {
        if (_implementation)
            throw exception("implementation already exists");
        _implementation.reset(new implementation(*this));
        return *_implementation;
    }

    void request::destroy_implementation() const {
        _implementation.reset();
    }

    request::implementation_base& request::get_implementation() const noexcept {
        if (!_implementation)
            throw exception("there is no implementation for this request");
        return *_implementation;
    }

    response request::operator()() const {
        log();
        create_implementation().perform();
        response _response = _implementation->read_response();
        destroy_implementation();
        return _response;
    }

    bool request::operator()(const string& file_name) const {
        throw exception("fetch requests can not have a file name");
    }

    request::implementation_base& request::download::create_implementation() const {
        if (download_implementation)
            throw exception("download implementation already exists");
        download_implementation.reset(new implementation(*this, file_name));
        return *download_implementation;
    }

    void request::download::destroy_implementation() const {
        download_implementation.reset();
    }

    request::implementation_base& request::download::get_implementation() const noexcept {
        if (!download_implementation)
            throw exception("there is no implementation for this download request");
        return *download_implementation;
    }

    response request::download::operator()() const {
        throw exception("download requests require a file name");
    }

    bool request::download::operator()(const string& _file_name) const {
        file_name = _file_name;
        create_implementation().perform();
        bool success = download_implementation->get_success();
        destroy_implementation();
        return success;
    }

    void request::download::remove_file() const {
        get_implementation().remove_file();
    }

    request request::get_relative(const string& path) const {
        regex url_pattern("(.*/)");
        smatch results;
        assert(regex_search(get_url(), results, url_pattern));
        return request(string(results[0]) + path);
    }

    ostream& operator<<(ostream& stream, const request& request) {
        return stream << request.get_url();
    }
}
