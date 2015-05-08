#ifndef CODESPAN_H
#define CODESPAN_H

#include "../TextHolder.h"


    class CodeSpan : public TextHolder {
        public:
        CodeSpan(const string& actualContents);
        virtual void interprete_to_html(ostream& out) const;
        virtual void writeAsOriginal(ostream& out) const;
        virtual void writeToken(ostream& out) const;
    };


#endif
