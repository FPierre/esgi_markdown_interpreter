#include "../../headers/tags/InlineHtmlComment.h"

InlineHtmlComment::InlineHtmlComment(const string& contents) : TextHolder(contents, false, 0) {

}

void InlineHtmlComment::write_token(ostream& out) const {
    out << "InlineHtmlComment: " << *text() << '\n';
}
