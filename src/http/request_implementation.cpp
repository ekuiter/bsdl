#include "request_implementation.hpp"
#include "client.hpp"
#include "exception.hpp"

using namespace std;

namespace http {
    request::implementation::implementation(const request& request):
            _request(request), writer(stream), curl(writer), performed(false), response_read(false) {
        client::instance().prepare(_request, *this);
    }

    void request::implementation::perform(bool multi) {
        if (_request.is_empty()) {
            cerr << "encountered empty fetch request" << endl;
            return;
        }
        if (!multi)
            client::instance().perform(_request);
        performed = true;
    }

    void request::implementation::remove_file() {
        throw exception("request has to be download request");
    }

    response request::implementation::read_response() {
        if (!performed)
            throw exception("request has not been performed yet");

        if (response_read)
            throw exception("response has already been read");
        response_read = true;

        string http_status;
        getline(stream, http_status);
        headers headers;
        stream >> headers;

        return response(
                curl.get_info<CURLINFO_HTTP_CODE>().get(),
                stream.str().substr(curl.get_info<CURLINFO_HEADER_SIZE>().get()),
                headers);
    }

    request::download::implementation::implementation(const request::download& _download_request, const string& _file_name):
            download_request(_download_request), file_name(_file_name),
            writer(file), curl(writer), performed(false), success(false) {
        if (file_name == "")
			throw exception(string("no file name set for ") + download_request.get_url());
        file.open(file_name, ios::binary);
        client::instance().prepare(download_request, *this);
    }

    void request::download::implementation::perform(bool multi) {
        if (download_request.is_empty()) {
            cerr << "encountered empty download request" << endl;
            return;
        }
        if (!multi)
            success = client::instance().perform(download_request);
        performed = true;
    }

    void request::download::implementation::remove_file() {
        file.close();
        remove(file_name.c_str());
    }

    bool request::download::implementation::get_success() {
        if (!performed)
            throw exception("request has not been performed yet");

        return success;
    }
}