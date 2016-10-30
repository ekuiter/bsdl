#pragma once

#include <iostream>
#include <unordered_map>
#include <sstream>
#include <curl_easy.h>
#include "../util/with_range.hpp"

using namespace std;

namespace http {
    class fields_base {
    protected:
        typedef unordered_map<string, string> container_type;
        
    private:       
        mutable container_type fields_map;
        
    protected:
        container_type& get_container() const {
            return fields_map;
        }

    public:
        fields_base() {}

        fields_base(const container_type& _fields_map) {
            fields_map = _fields_map;
        }

        string& operator[](const string& field) noexcept {
            return fields_map[field];
        }

        const string& operator[](const string& field) const noexcept {
            return fields_map.at(field);
        }

        int size() const noexcept {
            return fields_map.size();
        }
        
        friend ostream& operator<<(ostream& stream, const fields_base& fields) {
            curl::curl_easy curl;
            int i = 0;
            for (auto& field_pair : fields.fields_map) {
                string field = field_pair.first, value = field_pair.second;
                curl.escape(field);
                curl.escape(value);
                stream << field << "=" << value << (++i < fields.size() ? "&" : "");
            }
            return stream;
        }
        
        operator string() const {
            ostringstream stream;
            stream << *this;
            return stream.str();
        }
    };
    
    typedef util::with_range<fields_base> fields;
}