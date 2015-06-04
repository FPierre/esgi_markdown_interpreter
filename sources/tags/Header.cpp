#include "../../headers/tags/Header.h"

Header::Header(size_t level, const string& text) : TextHolder(text, true, cAmps|cAngles|cQuotes),
                                                   mLevel(level) {

}

void Header::write_token(ostream& out) const {
    out << "Header " << mLevel << ": " << *text() << '\n';
}

bool Header::inhibitParagraphs() const {
    return true;
}

void Header::preWrite(ostream& out) const {
    out << "<h" << mLevel << ">";
}

void Header::postWrite(ostream& out) const {
    out << "</h" << mLevel << ">\n";
}
