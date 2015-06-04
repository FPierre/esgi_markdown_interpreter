#include "../../headers/tags/EscapedCharacter.h"

EscapedCharacter::EscapedCharacter(char c) : mChar(c) {

}

void EscapedCharacter::interprete_to_html(ostream& out) const {
    out << mChar;
}

void EscapedCharacter::write_as_original(ostream& out) const {
    out << '\\' << mChar;
}

void EscapedCharacter::write_token(ostream& out) const {
    out << "EscapedCharacter: " << mChar << '\n';
}
