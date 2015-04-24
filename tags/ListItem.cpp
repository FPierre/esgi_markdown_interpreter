#include "ListItem.h"

namespace markdown {
    ListItem::ListItem(const TokenGroup& contents): Container(contents),
                                                    mInhibitParagraphs(true) {

    }

    void ListItem::inhibitParagraphs(bool set) {
        mInhibitParagraphs = set;
    }

    bool ListItem::inhibitParagraphs() const {
        return mInhibitParagraphs;
    }

    TokenPtr ListItem::clone(const TokenGroup& newContents) const {
        return TokenPtr(new ListItem(newContents));
    }

    string ListItem::containerName() const {
        return "ListItem";
    }

    void ListItem::preWrite(ostream& out) const {
        out << "<li>";
    }

    void ListItem::postWrite(ostream& out) const {
        out << "</li>\n";
    }
}
