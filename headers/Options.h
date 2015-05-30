#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

using namespace std;

class Options {
    private:
    string input_file;
    bool debug_mode;

    public:
    Options();

    bool read_options(int argc, char *argv[]);
    bool debug() const;
    string get_input_file() const;
};

#endif
