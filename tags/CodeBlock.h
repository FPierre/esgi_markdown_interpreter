#ifndef CODEBLOCK_H
#define CODEBLOCK_H

#include "../TextHolder.h"

namespace markdown {
    class CodeBlock : public TextHolder {
        public:
        CodeBlock(const string& actualContents);
        virtual void interprete_to_html(ostream& out) const;
        virtual void writeToken(ostream& out) const;
    };
}

#endif
