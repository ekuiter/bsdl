#include "request_chain.hpp"

namespace http {
    response request_chain::operator()() const {
        response response = initial_request();
        for (auto& request_builder : request_builders) {
            response = request_builder(response)();
        }
        return response;
    }
}
