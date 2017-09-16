#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include "settings.hpp"
#include "aggregators/bs/bs.hpp"
#include "aggregators/kx/kx.hpp"
#include "curses/terminal.hpp"
#include "curses/widgets.hpp"
#include "util/with_range.hpp"

#define THROW_UNIMPLEMENTED(return_type, fn, signature)         \
    return_type fn signature override {                         \
        throw runtime_error("calling unimplemented " #fn);      \
    }

using namespace std;
using namespace curses;

class app {
protected:
    static unique_ptr<app> _instance;
    
public:
    static app& instance();
    virtual const vector<aggregators::series*>& get_search_results() const = 0;
    virtual const aggregators::series* get_current_series() const = 0;
    virtual const string& get_series_search() const = 0;
    virtual void set_current_series(aggregators::series& series) = 0;
    virtual rectangle get_centered_bounds(int width = -1, int height = -1, int quarters = 3) = 0;
    virtual void initialize() = 0;
    virtual void set_title(const string& title, bool set_notice = false, string notice = "") = 0;
    virtual vector<aggregators::series*> search_series() = 0;
    virtual aggregators::series& choose_series(vector<aggregators::series*>& search_results,
        const string& prompt = "The following series were found:", const string& action = "Choose") = 0;
    virtual void display_series(aggregators::series& series) = 0;
    virtual void download_episodes(aggregators::download_selection& download_selection) = 0;
    virtual bool is_testing() = 0;
    
    virtual bool confirm(const string&, bool result) {
        return result;
    }
};
