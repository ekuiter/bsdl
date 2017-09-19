#include "plain_app.hpp"

plain_app::plain_app(): terminal(terminal::instance()), settings(settings::instance()) {
    if (settings.is_set("log_file") && settings["action"] != "show-log") {
        log_file.open(settings["log_file"]);
        terminal.get_stream(cout).set_log_file(&log_file);
        terminal.get_stream(cerr).set_log_file(&log_file);
        terminal.get_stream(clog).set_log_file(&log_file);
    }

    set_title("bsdl");
    clog << "bsdl initialized in " << settings["app"] << " mode." << endl
         << endl << "Settings:" << endl << settings << endl;
}

void plain_app::set_title(const string& title, bool set_notice, string notice) {
    if (!set_notice)
        clog << endl << title << endl << stream::write("-", title.length()) << endl;
}
