#pragma once

#include <iostream>
#include <vector>

using namespace std;

namespace aggregators {
    class subtitle {
        string name;
        static vector<subtitle*> preferred_subtitles;
        subtitle(const string& _name): name(_name) {}
        
    public:        
        static subtitle& instance(const string& name);
        static void set_preferred_subtitles(const vector<subtitle*>& _preferred_subtitles);
        static subtitle& get_preferred_subtitle(const vector<subtitle*>& subtitles);
        
        static const vector<subtitle*>& get_preferred_subtitles() {
            return preferred_subtitles;
        }
        
        const string& get_name() const {
            return name;
        }
    };
    
    ostream& operator<<(ostream& stream, const subtitle& subtitle);
}
