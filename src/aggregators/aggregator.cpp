#include "aggregator.hpp"
#include "bs/bs.hpp"
#include "kx/kx.hpp"
#include "mk/mk.hpp"
#include "pr/pr.hpp"

namespace aggregators {
    vector<aggregator*> aggregator::preferred_aggregators;
    
    aggregator& aggregator::instance(const string& name) {
        if (name == settings::get("aggregator_bs"))
            return bs::bs::instance();
        else if (name == settings::get("aggregator_kx"))
            return kx::kx::instance();
        else if (name == settings::get("aggregator_mk"))
            return mk::mk::instance();
        else if (name == settings::get("aggregator_pr"))
            return pr::pr::instance();
        else
            throw exception(string("unknown aggregator \"") + name + "\"");
    }
    
    void aggregator::set_preferred_aggregators(const vector<aggregator*>& _preferred_aggregators) {
        preferred_aggregators = _preferred_aggregators;
        int n = preferred_aggregators.size();
        if (n == 0)
            cout << "No preferred aggregators set." << endl;
        else if (n == 1)
            cout << "Preferred aggregator set to " << curses::color::get_accent_color() <<
                    *preferred_aggregators[0] << curses::color::previous << "." << endl;
        else {
            cout << "Preferred aggregators set to ";
            for (int i = 0; i < n; i++)
                cout << curses::color::get_accent_color() << *preferred_aggregators[i] <<
                        curses::color::previous << (i < n - 2 ? ", " : i == n - 2 ? " and " : ".\n");
        }
    }
    
    ostream& operator<<(ostream& stream, const aggregator& aggregator) {
        return stream << aggregator.get_name();
    }
}
