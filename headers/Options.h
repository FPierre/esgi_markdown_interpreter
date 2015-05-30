#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

using namespace std;

class Options {
    private:
    string input_file;
    bool debug_mode;
    bool test_mode;

    public:
    Options();

    bool readOptions(int argc, char *argv[]);
    void showHelp();
    bool debug() const;
    bool test() const;
    string get_input_file() const;
};

#endif
