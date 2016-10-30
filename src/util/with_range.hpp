#pragma once

#include <iostream>
#include <map>
#include <boost/range/adaptors.hpp>

namespace util {
    /*
     * Mixin for a Base class, adding range behaviour for container elements.
     * Base needs to conform to the following interface:
     * - it defines container_type as map<...>, vector<...> or similar
     * - it defines get_container() which returns the container inside Base
     */
    template<typename Base>
    class with_range : public Base {    
    public:
        typedef typename Base::container_type container_type;
        typedef typename container_type::iterator iterator;
        typedef typename container_type::const_iterator const_iterator;
        
        using Base::Base;

        const_iterator begin() const {
            return const_cast<const container_type&>(this->get_container()).begin();
        }

        const_iterator end() const {
            return const_cast<const container_type&>(this->get_container()).end();
        }

        iterator begin() {
            return this->get_container().begin();
        }

        iterator end() {
            return this->get_container().end();
        }

        BOOST_CONCEPT_ASSERT((boost_concepts::ReadableIteratorConcept<iterator>));
        BOOST_CONCEPT_ASSERT((boost_concepts::ReadableIteratorConcept<const_iterator>));
    };
}