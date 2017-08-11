#include "subtitle.hpp"
#include "exception.hpp"
#include <map>
#include <algorithm>
#include <sstream>
#include "../curses/terminal.hpp"

namespace aggregators {
    vector<subtitle*> subtitle::preferred_subtitles;
    
    subtitle& subtitle::instance(const string& name) {
        static map<string, subtitle> subtitles;
        auto allowed_subtitles = { "GerDub", "EngDub", "GerSub", "EngSub" };
        if (find(allowed_subtitles.begin(), allowed_subtitles.end(), name) != allowed_subtitles.end())
            return subtitles.insert({ name, subtitle(name) }).first->second;
        else
            throw exception(string("unknown subtitle \"") + name + "\"");
    }
    
    void subtitle::set_preferred_subtitles(const vector<subtitle*>& _preferred_subtitles) {
        preferred_subtitles = _preferred_subtitles;
        int n = preferred_subtitles.size();
        if (n == 0)
            cout << "No preferred subtitles set." << endl;
        else if (n == 1)
            cout << "Preferred subtitle set to " << curses::color::get_accent_color() <<
                    *preferred_subtitles[0] << curses::color::previous << "." << endl;
        else {
            cout << "Preferred subtitles set to ";
            for (int i = 0; i < n; i++)
                cout << curses::color::get_accent_color() << *preferred_subtitles[i] <<
                        curses::color::previous << (i < n - 2 ? ", " : i == n - 2 ? " and " : ".\n");
        }
    }

    subtitle& subtitle::get_preferred_subtitle(const vector<subtitle*>& subtitles) {
        for (auto& preferred_subtitle : preferred_subtitles) {
            vector<subtitle*>::const_iterator it;
            if ((it = find(subtitles.begin(), subtitles.end(), preferred_subtitle)) != subtitles.end()) {
                cout << "Using subtitle " << curses::color::get_accent_color() <<
                    **it << curses::color::previous << " (of ";
                int n = subtitles.size();
                for (int i = 0; i < n; i++)
                    cout << curses::color::get_accent_color() << *subtitles[i] <<
                        curses::color::previous << (i < n - 2 ? ", " : i == n - 2 ? " and " : ").\n");
                return **it;
            }
        }
        ostringstream err("there is no subtitle matching your preferences, choose from ", ios::ate);
        int i = 0;
        for (auto subtitle : subtitles) {
            err << *subtitle;
            if (++i < subtitles.size())
                err << ", ";
        }
        throw exception(err.str());
    }
    
    ostream& operator<<(ostream& stream, const subtitle& subtitle) {
        return stream << subtitle.get_name();
    }
}
