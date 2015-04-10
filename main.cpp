// g++ main.cpp Document.cpp tokens.cpp -lboost_regex

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/optional.hpp>
//#include <boost/regex.hpp> // For the 'test' option
#include "Options.h"
#include "Document.h"

using namespace std;

int main(int argc, char *argv[]) {
	Options cfg;

	if (!cfg.readOptions(argc, argv)) return 1;

//	if (cfg.test()) {
//		return 1;
//	}

	ifstream ifile;

	istream *in = &cin;

	if (!cfg.get_input_file().empty()) {
		cerr << "Reading file '" << cfg.get_input_file() << "'..." << endl;
		ifile.open(cfg.get_input_file().c_str());

		if (!ifile) {
			cerr << "Error: Can't open file." << endl;
			return 1;
		}
		else {
			in = &ifile;
		}
	}
	else {
		cerr << "Reading standard input..." << endl;
	}

	markdown::Document document;
	document.read(*in);

	// Si le mode débug est activé
	if (cfg.debug()) {
		document.writeTokens(cout);
	}
	// Sinon mode normal
    else {
		document.write(cout);
	}

	return 0;
}
