#include "../../headers/tags/BlankLine.h"


    BlankLine::BlankLine(const string& actualContents) : TextHolder(actualContents, false, 0) {

    }

    void BlankLine::writeToken(ostream& out) const {
        out << "BlankLine: " << *text() << '\n';
    }

    bool BlankLine::isBlankLine() const {
        return true;
    }

