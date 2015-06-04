#include "../../headers/tags/InlineHtmlContents.h"

InlineHtmlContents::InlineHtmlContents(const string& contents) : TextHolder(contents, false, cAmps|cAngles) {

}

void InlineHtmlContents::write_token(ostream& out) const {
    out << "InlineHtmlContents: " << *text() << '\n';
}
