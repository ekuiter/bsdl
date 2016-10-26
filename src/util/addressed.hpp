#pragma once

#include <iostream>
#include <boost/range/adaptor/transformed.hpp>

using namespace std;

namespace util {
    template<typename T>
    struct addressed : public boost::range_detail::transform_holder<function<T* (T&)>> {
        addressed():
            boost::range_detail::transform_holder<function<T* (T&)>>(
                boost::adaptors::transformed(
                    (function<T* (T&)>)
                        [](T& elem) {
                            return &elem;
                        }
                    )) {}
    };
}