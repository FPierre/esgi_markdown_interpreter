#ifndef INLINEHTMLCONTENT_H
#define INLINEHTMLCONTENT_H

#include "../TextHolder.h"


    class InlineHtmlContents : public TextHolder {
        public:
        InlineHtmlContents(const string& contents);
        virtual void writeToken(ostream& out) const;
    };


#endif
