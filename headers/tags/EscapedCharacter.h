#ifndef ESCAPEDCHARACTER_H
#define ESCAPEDCHARACTER_H

#include "../Token.h"

class EscapedCharacter : Token {
    public:
    EscapedCharacter(char c);

    virtual void interprete_to_html(ostream& out) const;
    virtual void writeAsOriginal(ostream& out) const;
    virtual void write_token(ostream& out) const;

    private:
    const char mChar;
};

#endif
