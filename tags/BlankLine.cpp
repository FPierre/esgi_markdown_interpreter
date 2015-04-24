#include "BlankLine.h"

namespace markdown {
    BlankLine::BlankLine(const string& actualContents) : TextHolder(actualContents, false, 0) {

    }

    void BlankLine::writeToken(ostream& out) const {
        out << "BlankLine: " << *text() << '\n';
    }

    bool BlankLine::isBlankLine() const {
        return true;
    }
}
