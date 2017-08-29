#include "test_util.hpp"
#include "../curses/terminal.hpp"

using namespace curses;

struct mock_terminal : public terminal {
    stream _stream;

    struct mock_error : public runtime_error {
        mock_error(const string& msg): runtime_error(msg + " called on mock terminal") {}
    };
    
    mock_terminal() {}

    static terminal& instance() {
        if (!_instance)
            _instance.reset(new mock_terminal());
        return *_instance;
    }

    MOCK_THROW(void, run, (function<void ()> fn));
    MOCK_THROW(input&, get_input, ());
    MOCK_THROW(char*, get_locale, ());
    MOCK_THROW(void, with_output, (ostream& _stream, function<void ()> fn));

    stream& get_stream(ostream& s) override {
        return _stream;
    }
};

struct mock_terminal_fixture {
    mock_terminal_fixture() {
        mock_terminal::instance();
    }
};

BOOST_GLOBAL_FIXTURE(mock_terminal_fixture);
