#ifndef CODESPAN_H
#define CODESPAN_H

#include "../TextHolder.h"

class CodeSpan : public TextHolder {
    public:
    CodeSpan(const string& actualContents);
    virtual void interprete_to_html(ostream& out) const;
    virtual void write_as_original(ostream& out) const;
    virtual void write_token(ostream& out) const;
};

#endif
