#pragma once

#include <iostream>
#include <map>
#include <utility>
#include <boost/range/adaptor/map.hpp>

using namespace boost::adaptors;

namespace util {
    /*
     * Mixin for a Base class, adding range behaviour for map values.
     * Base needs to conform to the following interface:
     * - it defines map_type as map<key_type, mapped_type>
     * - it defines get_map() which returns the map inside Base
     * - it defines load() which may lazy-load map values
     */
    template<typename Base>
    class with_map_range : public Base {    
    public:
        typedef typename Base::map_type map_type;
        typedef boost::iterators::transform_iterator<
            boost::range_detail::select_second_mutable<map_type>, typename map_type::iterator> iterator;
        typedef boost::iterators::transform_iterator<
            boost::range_detail::select_second_const<map_type>, typename map_type::const_iterator> const_iterator;
        
        using Base::Base;

        const_iterator begin() const {
            this->load();
            return (const_cast<const map_type&>(this->get_map()) | map_values).begin();
        }

        const_iterator end() const {
            this->load();
            return (const_cast<const map_type&>(this->get_map()) | map_values).end();
        }

        iterator begin() {
            this->load();
            return (this->get_map() | map_values).begin();
        }

        iterator end() {
            this->load();
            return (this->get_map() | map_values).end();
        }

        BOOST_CONCEPT_ASSERT((boost_concepts::ReadableIteratorConcept<iterator>));
        BOOST_CONCEPT_ASSERT((boost_concepts::ReadableIteratorConcept<const_iterator>));
    };
}