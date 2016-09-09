#include "headers.hpp"

namespace http {
    ostream& operator<<(ostream& stream, const headers& headers) {
        for (auto& header : headers)
            stream << header.first << ": " << header.second << "\r\n";
        return stream;
    }

    istream& operator>>(istream& stream, headers& headers) {
        string header, delimiter = ": ";
        while (getline(stream, header) && header != "\r") {
            auto delimiter_index = header.find(delimiter);
            string field = header.substr(0, delimiter_index),
                    value = header.substr(delimiter_index + delimiter.size(), header.size() - field.size() - delimiter.size() - 1);
            headers[field] = value;
        }
        return stream;
    }
}
