#pragma once

#include <iostream>
#include <map>
#include <boost/range/adaptor/map.hpp>

using namespace boost::adaptors;

namespace util {
    template<typename Base>
    class with_map_range : public Base {    
    public:
        typedef typename Base::map_type map_type;
        typedef boost::iterators::transform_iterator<
            boost::range_detail::select_second_mutable<map_type>, typename map_type::iterator> iterator;
        typedef boost::iterators::transform_iterator<
            boost::range_detail::select_second_const<map_type>, typename map_type::const_iterator> const_iterator;

        const_iterator begin() const {
            return (const_cast<const map_type&>(this->get_map()) | map_values).begin();
        }

        const_iterator end() const {
            return (const_cast<const map_type&>(this->get_map()) | map_values).end();
        }

        iterator begin() {
            return (this->get_map() | map_values).begin();
        }

        iterator end() {
            return (this->get_map() | map_values).end();
        }

        BOOST_CONCEPT_ASSERT((boost_concepts::ReadableIteratorConcept<iterator>));
        BOOST_CONCEPT_ASSERT((boost_concepts::ReadableIteratorConcept<const_iterator>));
    };
}