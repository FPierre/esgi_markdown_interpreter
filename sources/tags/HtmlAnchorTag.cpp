#include "../../headers/tags/HtmlAnchorTag.h"

HtmlAnchorTag::HtmlAnchorTag(const string& url, const string& title) : TextHolder("<a href=\""+encode_string(url, cQuotes|cAmps)+"\""
        +(title.empty() ? string() : " title=\""+encode_string(title, cQuotes|cAmps)+"\"")
        +">", false, 0) {

}

void HtmlAnchorTag::write_token(ostream& out) const {
    out << "HtmlAnchorTag: " << *text() << '\n';
}
