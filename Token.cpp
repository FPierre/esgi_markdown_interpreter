#include <ostream>
#include <string>
#include "Token.h"

using namespace std;

namespace markdown {
    Token::Token() {

    }

    void Token::writeAsOriginal(ostream& out) const {
        interprete_to_html(out);
    }

    void Token::writeToken(size_t indent, ostream& out) const {
        out << string(indent * 2, ' ');
        writeToken(out);
    }

    optional<TokenGroup> Token::processSpanElements(const LinkIds& idTable) {
        return none;
    }

    optional<const string&> Token::text() const {
        return none;
    }

    bool Token::canContainMarkup() const {
        return false;
    }

    bool Token::isBlankLine() const {
        return false;
    }

    bool Token::isContainer() const {
        return false;
    }

    bool Token::isUnmatchedOpenMarker() const {
        return false;
    }

    bool Token::isUnmatchedCloseMarker() const {
        return false;
    }

    bool Token::isMatchedOpenMarker() const {
        return false;
    }

    bool Token::isMatchedCloseMarker() const {
        return false;
    }

    bool Token::inhibitParagraphs() const {
        return false;
    }

    void Token::preWrite(ostream& out) const {

    }

    void Token::postWrite(ostream& out) const {

    }
}
