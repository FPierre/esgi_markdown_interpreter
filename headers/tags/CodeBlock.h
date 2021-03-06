#ifndef CODEBLOCK_H
#define CODEBLOCK_H

#include "../TextHolder.h"

class CodeBlock : public TextHolder {
    public:
    CodeBlock(const string& actualContents);

    virtual void interprete_to_html(ostream& out) const;
    virtual void write_token(ostream& out) const;
};

#endif
