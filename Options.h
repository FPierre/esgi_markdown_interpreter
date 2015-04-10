#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

using namespace std;

class Options {
    public:
    Options();
    bool readOptions(int argc, char *argv[]);
    void showHelp();
    bool debug() const;
    bool test() const;
    string get_input_file() const;

    private:
    string input_file;
    bool debug_mode;
    bool test_mode;
};

Options::Options():debug_mode(false),
                   test_mode(false) {

}

/**
 * Gère l'appel des options de la ligne de commande
 */
bool Options::readOptions(int argc, char *argv[]) {
    bool help = false;
    bool err = false;

    for (int x = 1; x < argc && !help && !err; ++x) {
        if (argv[x][0] == '-' && argv[x][1] == '-' && argv[x][2] != 0) {
            // It's a full-word option.
            string opt(argv[x] + 2);

            if (opt == "debug") {
                debug_mode = true;
            }
            else if (opt == "test") {
                test_mode = true;
            }
            else if (opt == "help") {
                help = true;
            }
            else {
                err = true;

                cerr << "Unknown option " << argv[x] << ", use -? for help." << endl;
            }
        }
        else if (argv[x][0] == '-') {
            // It's one or more option flags.
            const char *i = argv[x] + 1;

            while (*i != 0 && !help && !err) {
                switch (*i) {
                    case '?': help = true; break;
                    case 'd': debug_mode = true; break;
                    default:
                        err = true;

                        cerr << "Uknown option flag '" << *i << "', use -? for help." << endl;
                }

                ++i;
            }
        }
        else {
            // It's a parameter.
            if (input_file.empty()) {
                input_file = argv[x];
            }
            else {
                err = true;

                cerr << "Too many parameters. Already had '" << input_file << "', found '" << argv[x] << "' too. Use -? for help." << endl;
            }
        }
    }

    if (help) {
        showHelp();
        return false;
    }
    else if (err) {
        return false;
    }
    else {
        return true;
    }
}

void Options::showHelp() {
    const char *cHelpScreen=
        "This program converts input (from an input file or stdin) from Markdown syntax\n"
        "to HTML using the cpp-markdown library.\n"
        "\n"
        "Usage:\n"
        "    cpp-markdown [<option>...] [input-file]\n"
        "\n"
        "Available options are:\n"
        "    -?, --help      Show this screen.\n"
        "    -d, --debug     Show tokens instead of HTML output.\n";
    cerr << endl << cHelpScreen << endl;
}

bool Options::debug() const {
    return debug_mode;
}

bool Options::test() const {
    return test_mode;
}

string Options::get_input_file() const {
    return input_file;
}

#endif
