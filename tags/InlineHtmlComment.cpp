#include "InlineHtmlComment.h"

namespace markdown {
    InlineHtmlComment::InlineHtmlComment(const string& contents) : TextHolder(contents, false, 0) {

    }

    void InlineHtmlComment::writeToken(ostream& out) const {
        out << "InlineHtmlComment: " << *text() << '\n';
    }
}
