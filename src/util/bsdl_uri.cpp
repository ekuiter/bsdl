#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>

using namespace std;

namespace util {
    // see stackoverflow.com/q/154536
    namespace bsq = boost::spirit::qi;
    namespace bk = boost::spirit::karma;
    bsq::int_parser<unsigned char, 16, 2, 2> hex_byte;
    
    template <typename InputIterator>
    struct unescaped_string : bsq::grammar<InputIterator, string(char const *)> {
        unescaped_string() : unescaped_string::base_type(unesc_str) {
            unesc_char.add("+", ' ');
            unesc_str = *(unesc_char | "%" >> hex_byte | bsq::char_);
        }

        bsq::rule<InputIterator, string(char const *)> unesc_str;
        bsq::symbols<char const, char const> unesc_char;
    };

    template <typename OutputIterator>
    struct escaped_string : bk::grammar<OutputIterator, string(char const *)> {
        escaped_string() : escaped_string::base_type(esc_str) {
            esc_str = *(bk::char_("a-zA-Z0-9_.~-") | "%" << bk::right_align(2,0)[bk::hex]);
        }
        
        bk::rule<OutputIterator, string(char const *)> esc_str;
    };

    string encode_uri(const string &input) {
        typedef back_insert_iterator<string> sink_type;
        string retVal;
        retVal.reserve(input.size() * 3);
        sink_type sink(retVal);
        char const *start = "";

        escaped_string<sink_type> g;
        if (!bk::generate(sink, g(start), input))
            retVal = input;
        return retVal;
    }

    string decode_uri(const string &input) {
        string retVal;
        retVal.reserve(input.size());
        typedef string::const_iterator iterator_type;

        char const *start = "";
        iterator_type beg = input.begin();
        iterator_type end = input.end();
        unescaped_string<iterator_type> p;

        if (!bsq::parse(beg, end, p(start), retVal))
            retVal = input;
        return retVal;
    }
}
