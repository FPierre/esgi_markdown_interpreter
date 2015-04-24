#ifndef TOKENS_H
#define TOKENS_H

#include <vector>
#include "Document.h"
#include "Token.h"

namespace markdown {
    size_t isValidTag(const string& tag, bool nonBlockFirst = false);

    class BoldOrItalicMarker: public Token {
        public:
        BoldOrItalicMarker(bool open, char c, size_t size): mOpenMarker(open),
            mTokenCharacter(c), mSize(size), mMatch(0), mCannotMatch(false),
            mDisabled(false), mId(-1) { }

        virtual bool isUnmatchedOpenMarker() const { return (mOpenMarker && mMatch==0 && !mCannotMatch); }
        virtual bool isUnmatchedCloseMarker() const { return (!mOpenMarker && mMatch==0 && !mCannotMatch); }
        virtual bool isMatchedOpenMarker() const { return (mOpenMarker && mMatch!=0); }
        virtual bool isMatchedCloseMarker() const { return (!mOpenMarker && mMatch!=0); }
        virtual void interprete_to_html(ostream& out) const;
        virtual void writeToken(ostream& out) const;

        bool isOpenMarker() const { return mOpenMarker; }
        char tokenCharacter() const { return mTokenCharacter; }
        size_t size() const { return mSize; }
        bool matched() const { return (mMatch!=0); }
        BoldOrItalicMarker* matchedTo() const { return mMatch; }
        int id() const { return mId; }

        void matched(BoldOrItalicMarker *match, int id=-1) { mMatch=match; mId=id; }
        void cannotMatch(bool set) { mCannotMatch=set; }
        void disable() { mCannotMatch=mDisabled=true; }

        private:
        bool mOpenMarker; // Otherwise it's a close-marker
        char mTokenCharacter; // Underscore or asterisk
        size_t mSize; // 1=italics, 2=bold, 3=both
        BoldOrItalicMarker* mMatch;
        bool mCannotMatch;
        bool mDisabled;
        int mId;
    };

    class Image : public Token {
        public:
        Image(const string& altText, const string& url, const string& title): mAltText(altText), mUrl(url), mTitle(title) { }
        virtual void interprete_to_html(ostream& out) const;
        virtual void writeToken(ostream& out) const { out << "Image: " << mUrl << '\n'; }

        private:
        const string mAltText, mUrl, mTitle;
    };
}

#endif
