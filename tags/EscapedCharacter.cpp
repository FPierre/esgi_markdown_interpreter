#include "EscapedCharacter.h"

namespace markdown {
    EscapedCharacter::EscapedCharacter(char c) : mChar(c) {

    }

    void EscapedCharacter::interprete_to_html(ostream& out) const {
        out << mChar;
    }

    void EscapedCharacter::writeAsOriginal(ostream& out) const {
        out << '\\' << mChar;
    }

    void EscapedCharacter::writeToken(ostream& out) const {
        out << "EscapedCharacter: " << mChar << '\n';
    }
}
