#ifndef CONTAINER_H
#define CONTAINER_H

#include "Token.h"

class Container : public Token {
    public:
    Container(const TokenGroup& contents = TokenGroup());

    const TokenGroup& subTokens() const;
    void appendSubtokens(TokenGroup& tokens);
    void swapSubtokens(TokenGroup& tokens);

    virtual bool is_container() const;
    virtual void interprete_to_html(ostream& out) const;
    virtual void write_token(ostream& out) const;
    virtual void write_token(size_t indent, ostream& out) const;
    virtual optional<TokenGroup> process_span_elements(const LinkIds& idTable);
    virtual TokenPtr clone(const TokenGroup& newContents) const;
    virtual string container_name() const;

    protected:
    TokenGroup mSubTokens;
    bool mParagraphMode;
};

#endif
