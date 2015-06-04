#ifndef TOKEN_H
#define TOKEN_H

#include <ostream>
#include <string>
#include <vector>
#include "Document.h"
#include "LinkIds.h"

using namespace std;

typedef TokenGroup::iterator TokenGroupIter;
typedef TokenGroup::const_iterator CTokenGroupIter;

class Token {
    public:
    Token();

    virtual void interprete_to_html(ostream&) const = 0;
    virtual void writeAsOriginal(ostream& out) const;
    virtual void write_token(ostream& out) const = 0;
    virtual void write_token(size_t indent, ostream& out) const;
    virtual optional<TokenGroup> processSpanElements(const LinkIds& idTable);
    virtual optional<const string&> text() const;
    virtual bool canContainMarkup() const;
    virtual bool is_blanck_line() const;
    virtual bool isContainer() const;
    virtual bool isUnmatchedOpenMarker() const;
    virtual bool isUnmatchedCloseMarker() const;
    virtual bool isMatchedOpenMarker() const;
    virtual bool isMatchedCloseMarker() const;
    virtual bool inhibitParagraphs() const;

    protected:
    virtual void preWrite(ostream& out) const;
    virtual void postWrite(ostream& out) const;
};

#endif
