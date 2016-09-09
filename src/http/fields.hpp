#pragma once

#include <iostream>
#include <unordered_map>
#include <sstream>

using namespace std;

namespace http {
    class fields {
        unordered_map<string, string> fields_map;

    public:
        fields() {}

        fields(const unordered_map<string, string>& _fields_map) {
            fields_map = _fields_map;
        }

        string& operator[](const string& field) noexcept {
            return fields_map[field];
        }

        const string& operator[](const string& field) const noexcept {
            return fields_map.at(field);
        }

        unordered_map<string, string>::const_iterator begin() const {
            return fields_map.begin();
        }

        unordered_map<string, string>::const_iterator end() const {
            return fields_map.end();
        }

        int size() const noexcept {
            return fields_map.size();
        }

        operator string() const;
    };

    ostream& operator<<(ostream& stream, const fields& fields);
}