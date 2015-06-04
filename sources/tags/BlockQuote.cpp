#include "../../headers/tags/BlockQuote.h"

BlockQuote::BlockQuote(const TokenGroup& contents): Container(contents) {

}

TokenPtr BlockQuote::clone(const TokenGroup& newContents) const {
    return TokenPtr(new BlockQuote(newContents));
}

string BlockQuote::container_name() const {
    return "BlockQuote";
}

void BlockQuote::pre_write(ostream& out) const {
    out << "<blockquote>\n";
}

void BlockQuote::post_write(ostream& out) const {
    out << "\n</blockquote>\n";
}
