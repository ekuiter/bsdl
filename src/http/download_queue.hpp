#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "multi_request.hpp"

using namespace std;

namespace http {
    template <typename T>
    class download_queue {
        vector<T> downloads;
        set<int> scheduled_downloads;
        multi::request::download multi_request;
        int parallel_transfers;

    public:
        download_queue(int _parallel_transfers = 3): parallel_transfers(_parallel_transfers) {}

        void push(const T& download) {
            downloads.push_back(download);
            scheduled_downloads.insert(downloads.size() - 1);
        }

        void abort(const T& download) {
            auto it = find(downloads.begin(), downloads.end(), download);
            if (it == downloads.end())
                throw exception("download is not in the download queue");
            string message = download.get_message();
            if (message == "Done" || message == "Aborted" || message == "Unavailable")
                return;
            if (message == "Queued")
                scheduled_downloads.erase(it - downloads.begin());
            it->set_abort(true);
        }

        void operator()() {
            while (!scheduled_downloads.empty() || multi_request())
                if (!scheduled_downloads.empty() && multi_request() < parallel_transfers) {
                    int idx = *scheduled_downloads.begin();
                    T& download = downloads[idx];
                    scheduled_downloads.erase(idx);
                    download.get_download_request();
                    if (download.get_download_request().is_empty())
                        download.set_message("Unavailable");
                    else
                        multi_request.add(download.get_download_request());
                }
        }

        typename vector<T>::const_iterator begin() const {
            return downloads.begin();
        }

        typename vector<T>::const_iterator end() const {
            return downloads.end();
        }

        typename vector<T>::iterator begin() {
            return downloads.begin();
        }

        typename vector<T>::iterator end() {
            return downloads.end();
        }
    };
}
