#include "../../headers/tags/HtmlAnchorTag.h"

HtmlAnchorTag::HtmlAnchorTag(const string& url, const string& title) : TextHolder("<a href=\""+encodeString(url, cQuotes|cAmps)+"\""
        +(title.empty() ? string() : " title=\""+encodeString(title, cQuotes|cAmps)+"\"")
        +">", false, 0) {

}

void HtmlAnchorTag::writeToken(ostream& out) const {
    out << "HtmlAnchorTag: " << *text() << '\n';
}
