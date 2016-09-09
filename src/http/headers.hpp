#pragma once

#include <iostream>
#include <unordered_map>

using namespace std;

namespace http {
    class headers {
        unordered_map<string, string> headers_map;

    public:
        headers() {}

        headers(const unordered_map<string, string>& _headers_map) {
            headers_map = _headers_map;
        }

        string& operator[](const string& field) noexcept {
            return headers_map[field];
        }

        const string& operator[](const string& field) const noexcept {
            return headers_map.at(field);
        }

        unordered_map<string, string>::const_iterator begin() const {
            return headers_map.begin();
        }

        unordered_map<string, string>::const_iterator end() const {
            return headers_map.end();
        }

        int size() const noexcept {
            return headers_map.size();
        }
    };

    ostream& operator<<(ostream& stream, const headers& headers);
    istream& operator>>(istream& stream, headers& headers);
}