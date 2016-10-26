#include "provider.hpp"
#include "provider_v.hpp"
#include "provider_s.hpp"
#include "../settings.hpp"
#include "../curses/terminal.hpp"

namespace providers {
    vector<provider*> provider::preferred_providers;

    provider& provider::instance(const string& name, bool should_throw) {
        if (name == "Unavailable")
            return unknown::instance(name);
        else if (name == settings::get("provider_v"))
            return v::instance();
        else if (name == settings::get("provider_s"))
            return s::instance();
        else {
            if (should_throw)
                throw exception(string("unknown provider \"") + name + "\"");
            else
                return unknown::instance(name);
        }
    }

    void provider::set_preferred_providers(const vector<provider*>& _preferred_providers) {
        preferred_providers = _preferred_providers;
        int n = preferred_providers.size();
        if (n == 0)
            cout << "No preferred providers set." << endl;
        else if (n == 1) {
            cout << "Preferred provider set to " << curses::color::get_accent_color() <<
                    *preferred_providers[0] << curses::color::previous << "." << endl;
        } else {
            cout << "Preferred providers set to ";
            for (int i = 0; i < n; i++)
                cout << curses::color::get_accent_color() << *preferred_providers[i] <<
                        curses::color::previous << (i < n - 2 ? ", " : i == n - 2 ? " and " : ".\n");
        }
    }

    ostream& operator<<(ostream& stream, const provider& provider) {
        return stream << provider.get_name();
    }
}