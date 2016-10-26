#pragma once

#include "provider.hpp"
#include "../settings.hpp"
#include "../util/platform.hpp"
#include <iostream>

using namespace std;
using namespace http;

namespace providers {
    class provider::youtube_dl : public provider {    
        mutable bool youtube_dl_installed = false;
        mutable bool youtube_dl_updated = false;

        void install_youtube_dl() const {
            if (youtube_dl_installed)
                return;
            try {
                string version = util::platform::exec("youtube-dl --version");
                boost::trim(version);
                cout << "youtube-dl is installed (" << version << ")." << endl;
            } catch (util::platform::exec_failed) {
                try {
                    cout << "Installing youtube-dl ..." << endl;
                    util::platform::exec("brew install youtube-dl");
                } catch (util::platform::exec_failed) {
                    try {
                        cout << "Installing Homebrew ..." << endl;
                        util::platform::exec("/usr/bin/ruby -e \"$(curl -fsSL "
                                "https://raw.githubusercontent.com/Homebrew/install/master/install)\"");
                        cout << "Homebrew has been installed." << endl;
                        try {
                            cout << "Installing youtube-dl ..." << endl;
                            util::platform::exec("brew install youtube-dl");
                        } catch (util::platform::exec_failed) {
                            throw runtime_error("youtube-dl could not be installed.\nTry again or install it manually.");
                        }
                    } catch (util::platform::exec_failed) {
                        throw runtime_error("Homebrew could not be installed.\nTry again or install it manually.");
                    }
                }
                string version = util::platform::exec("youtube-dl --version");
                boost::trim(version);
                cout << "youtube-dl has been installed (" << version << ")." << endl;
            }
            youtube_dl_installed = true;
        }

        void update_youtube_dl(const request& _request) const {
            if (youtube_dl_updated)
                throw not_found(_request);
            try {
                cout << "Video not found, updating youtube-dl ..." << endl;
                util::platform::exec("sudo youtube-dl -U");
                cout << "youtube-dl has been updated." << endl;
            } catch (util::platform::exec_failed) {
                throw not_found(_request);
            }
            youtube_dl_updated = true;
        }

    protected:
        youtube_dl(const string& name): provider(name) {}

    public:
        request::download fetch(const request& _request) const override {
            install_youtube_dl();
            try {
                return request::download(
                    util::platform::exec(string("youtube-dl --get-url ") + _request.get_url())
                );
            } catch (util::platform::exec_failed) {
                update_youtube_dl(_request);
                return fetch(_request);
            }
        }
    };
}
