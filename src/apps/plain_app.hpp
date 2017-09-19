#pragma once

#include "../app.hpp"

class plain_app : public app {
protected:
    terminal& terminal;
    settings& settings;
    ofstream log_file;
    
    plain_app();

public:
    void set_title(const string& title, bool set_notice = false, string notice = "") override;

    bool confirm(const string& msg, bool result) override {
        return result;
    }

    THROW_UNIMPLEMENTED(rectangle, get_centered_bounds, (int width = -1, int height = -1, int quarters = 3));
};
