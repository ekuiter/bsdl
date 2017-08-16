#include "mock_app.hpp"

struct mock_app_fixture {
    mock_app_fixture() {
        mock_app::instance();
    }
};

BOOST_GLOBAL_FIXTURE(mock_app_fixture);
