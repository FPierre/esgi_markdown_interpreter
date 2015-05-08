#ifndef TEXTHOLDER_H
#define TEXTHOLDER_H

#include <string>
#include "Token.h"

using namespace std;


    enum EncodingFlags { cAmps = 0x01, cDoubleAmps = 0x02, cAngles = 0x04, cQuotes = 0x08 };

    class TextHolder : public Token {
        private:
        const string mText;
        const bool mCanContainMarkup;
        const int mEncodingFlags;

        public:
        TextHolder(const string& text, bool canContainMarkup, unsigned int encodingFlags);
        string encodeString(const string& src, int encodingFlags);
        virtual void interprete_to_html(ostream& out) const;
        virtual void writeToken(ostream& out) const;
        virtual optional<const string&> text() const;
        virtual bool canContainMarkup() const;
    };


#endif
