#include "../../headers/tags/BlankLine.h"

BlankLine::BlankLine(const string& actualContents) : TextHolder(actualContents, false, 0) {

}

void BlankLine::write_token(ostream& out) const {
    out << "BlankLine: " << *text() << '\n';
}

bool BlankLine::is_blank_line() const {
    return true;
}
