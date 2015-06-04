#ifndef BOLDORITALICMARKER_H
#define BOLDORITALICMARKER_H

#include "../Token.h"

class BoldOrItalicMarker : public Token {
    public:
    BoldOrItalicMarker(bool open, char c, size_t size);

    virtual bool isUnmatchedOpenMarker() const;
    virtual bool isUnmatchedCloseMarker() const;
    virtual bool isMatchedOpenMarker() const;
    virtual bool isMatchedCloseMarker() const;
    virtual void interprete_to_html(ostream& out) const;
    virtual void write_token(ostream& out) const;

    bool isOpenMarker() const;
    char tokenCharacter() const;
    size_t size() const;
    bool matched() const;
    BoldOrItalicMarker* matchedTo() const;
    int id() const;

    void matched(BoldOrItalicMarker *match, int id = -1);
    void cannotMatch(bool set);
    void disable();

    private:
    bool mOpenMarker;     // Otherwise it's a close-marker
    char mTokenCharacter; // Underscore or asterisk
    size_t mSize;         // 1 = italics, 2 = bold, 3 = both
    BoldOrItalicMarker* mMatch;
    bool mCannotMatch;
    bool mDisabled;
    int mId;
};

#endif
