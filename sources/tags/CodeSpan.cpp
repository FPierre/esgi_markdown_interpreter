#include "../../headers/tags/CodeSpan.h"

CodeSpan::CodeSpan(const string& actualContents) : TextHolder(actualContents, false, cDoubleAmps|cAngles|cQuotes) {

}

void CodeSpan::interprete_to_html(ostream& out) const {
    out << "<code>";
    TextHolder::interprete_to_html(out);
    out << "</code>";
}

void CodeSpan::write_as_original(ostream& out) const {
    out << '`' << *text() << '`';
}

void CodeSpan::write_token(ostream& out) const {
    out << "CodeSpan: " << *text() << '\n';
}
