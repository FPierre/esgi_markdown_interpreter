#include "../../headers/tags/CodeSpan.h"

CodeSpan::CodeSpan(const string& actualContents) : TextHolder(actualContents, false, cDoubleAmps|cAngles|cQuotes) {

}

void CodeSpan::interprete_to_html(ostream& out) const {
    out << "<code>";
    TextHolder::interprete_to_html(out);
    out << "</code>";
}

void CodeSpan::writeAsOriginal(ostream& out) const {
    out << '`' << *text() << '`';
}

void CodeSpan::writeToken(ostream& out) const {
    out << "CodeSpan: " << *text() << '\n';
}
