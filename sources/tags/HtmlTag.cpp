#include "../../headers/tags/HtmlTag.h"

HtmlTag::HtmlTag(const string& contents): TextHolder(contents, false, cAmps|cAngles) {

}

void HtmlTag::write_token(ostream& out) const {
    out << "HtmlTag: " << *text() << '\n';
}

void HtmlTag::preWrite(ostream& out) const {
    out << '<';
}

void HtmlTag::postWrite(ostream& out) const {
    out << '>';
}
