#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <numeric>

using namespace std;

class option {
    static vector<option> options;
    string key;
    vector<string> arg_names;
    int args, max_args = 2;
    typedef function<void (string, string)> handler_func;
    typedef function<void ()> modify_func;
    handler_func on_validate, on_process;
    modify_func on_modify;
    string description;

    string detailed_switch() const {
        return string("--") + key;
    }

    string abbreviated_switch() const {
        return string("-") + key[0];
    }

    template<typename T>
    bool matches(T is_arg) {
        return is_arg(detailed_switch(), args) || is_arg(abbreviated_switch(), args);
    }

    template<typename T>
    bool call_handler(handler_func handler, T next_arg) {
        if (args == 0)
            handler("", "");
        else if (args == 1) {
            string arg1 = next_arg();
            handler(arg1, "");
        } else if (args == 2) {
            string arg1 = next_arg();
            string arg2 = next_arg();
            handler(arg1, arg2);
        } else
            throw runtime_error("invalid number of arguments");
        return true;
    }
    
public:
    static void setup_options();

    static vector<option>& get_options() {
        return options;
    }

    static vector<option*> get_modifiable_options() {
        vector<option*> modifiable_options;
        for (auto& option : options)
            if (option.on_modify)
                modifiable_options.push_back(&option);
        return modifiable_options;
    }
    
    option(const string& _key, const vector<string>& _arg_names, handler_func _on_validate, handler_func _on_process,
           modify_func _on_modify = nullptr, const string& _description = ""):
        key(_key), arg_names(_arg_names), args(_arg_names.size()), on_validate(_on_validate), on_process(_on_process),
        on_modify(_on_modify), description(_description) {
        if (args > max_args)
            throw runtime_error("maximum number of arguments exceeded");
    }

    template<typename T, typename U>
    bool validate(T is_arg, U next_arg) {
        return matches(is_arg) ? call_handler(on_validate, next_arg) : false;
    }

    template<typename T, typename U>
    bool process(T is_arg, U next_arg) {
        return matches(is_arg) ? call_handler(on_process, next_arg) : false;
    }

    void modify() {
        if (on_modify)
            on_modify();
    }

    string get_usage() const {
        string usage = detailed_switch();
        for (auto arg_name : arg_names)
            usage += " " + arg_name;
        return usage;
    }

    const string& get_description() const {
        return description == "" ? key : description;
    }
};

ostream& operator<<(ostream& stream, const option& option);
