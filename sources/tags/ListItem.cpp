#include "../../headers/tags/ListItem.h"

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

void ListItem::pre_write(ostream& out) const {
    out << "<li>";
}

void ListItem::post_write(ostream& out) const {
    out << "</li>\n";
}
