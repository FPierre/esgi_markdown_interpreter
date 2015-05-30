#ifndef CONTAINER_H
#define CONTAINER_H

#include "Token.h"


    class Container : public Token {
        public:
        Container(const TokenGroup& contents = TokenGroup());

        const TokenGroup& subTokens() const;
        void appendSubtokens(TokenGroup& tokens);
        void swapSubtokens(TokenGroup& tokens);

        virtual bool isContainer() const;
        virtual void interprete_to_html(ostream& out) const;
        virtual void writeToken(ostream& out) const;
        virtual void writeToken(size_t indent, ostream& out) const;
        virtual optional<TokenGroup> processSpanElements(const LinkIds& idTable);
        virtual TokenPtr clone(const TokenGroup& newContents) const;
        virtual string containerName() const;

        protected:
        TokenGroup mSubTokens;
        bool mParagraphMode;
    };


#endif
