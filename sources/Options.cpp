#include <string>
#include <iostream>
#include "../headers/Options.h"

using namespace std;

Options::Options() : debug_mode(false) {

}

/**
 * Gère les options de la ligne de commande
 */
bool Options::read_options(int argc, char *argv[]) {
    bool error = false;

    // Pour toutes les options de la ligne de commande
    for (int i = 1; i < argc && !error; i++) {
        // Si c'est une option
        if (argv[i][0] == '-' && argv[i][1] == '-' && argv[i][2] != 0) {
            string opt(argv[i] + 2);

            if (opt == "debug") {
                debug_mode = true;
            }
            else {
                error = true;

                cerr << "Unknown option " << argv[i] << endl;
            }
        }
        // Sinon si c'est une option sous forme de flag
        else if (argv[i][0] == '-') {
            const char *j = argv[i] + 1;

            while (*j != 0 && !error) {
                switch (*j) {
                    case 'd': debug_mode = true; break;
                    default:
                        error = true;

                        cerr << "Uknown option flag '" << *j << endl;
                }

                j++;
            }
        }
        // Sinon c'est un paramètre
        else {
            if (input_file.empty()) {
                input_file = argv[i];
            }
            else {
                error = true;

                cerr << "Too many parameters. Already had '" << input_file << "', found '" << argv[i] << endl;
            }
        }
    }

    if (error) {
        return false;
    }
    else {
        return true;
    }
}

bool Options::debug() const {
    return debug_mode;
}

string Options::get_input_file() const {
    return input_file;
}
