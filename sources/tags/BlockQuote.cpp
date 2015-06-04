#include "../../headers/tags/BlockQuote.h"

BlockQuote::BlockQuote(const TokenGroup& contents): Container(contents) {

}

TokenPtr BlockQuote::clone(const TokenGroup& newContents) const {
    return TokenPtr(new BlockQuote(newContents));
}

string BlockQuote::containerName() const {
    return "BlockQuote";
}

void BlockQuote::preWrite(ostream& out) const {
    out << "<blockquote>\n";
}

void BlockQuote::postWrite(ostream& out) const {
    out << "\n</blockquote>\n";
}
