#include "../../headers/tags/Paragraph.h"

Paragraph::Paragraph() {

}

Paragraph::Paragraph(const TokenGroup& contents) : Container(contents) {

}

TokenPtr Paragraph::clone(const TokenGroup& newContents) const {
    return TokenPtr(new Paragraph(newContents));
}

string Paragraph::containerName() const {
    return "Paragraph";
}

void Paragraph::pre_write(ostream& out) const {
    out << "<p>";
}

void Paragraph::post_write(ostream& out) const {
    out << "</p>\n\n";
}
