#pragma once

#include <iostream>
#include <unordered_map>
#include "../util/with_range.hpp"

using namespace std;

namespace http {
    class headers_base {
    public:
        typedef unordered_map<string, string> container_type;
        
    private:        
        mutable unordered_map<string, string> headers_map;
        
    protected:
        container_type& get_container() const {
            return headers_map;
        } 

    public:
        headers_base() {}

        headers_base(const container_type& _headers_map) {
            headers_map = _headers_map;
        }

        string& operator[](const string& field) noexcept {
            return headers_map[field];
        }

        const string& operator[](const string& field) const noexcept {
            return headers_map.at(field);
        }

        int size() const noexcept {
            return headers_map.size();
        }
        
        friend ostream& operator<<(ostream& stream, const headers_base& headers) {
            for (auto& header : headers.headers_map)
                stream << header.first << ": " << header.second << "\r\n";
            return stream;
        }

        friend istream& operator>>(istream& stream, headers_base& headers) {
            string header, delimiter = ": ";
            while (getline(stream, header) && header != "\r") {
                auto delimiter_index = header.find(delimiter);
                string field = header.substr(0, delimiter_index),
                        value = header.substr(delimiter_index + delimiter.size(), header.size() - field.size() - delimiter.size() - 1);
                headers[field] = value;
            }
            return stream;
        }
    };
    
    typedef util::with_range<headers_base> headers;
}