#ifndef INLINEHTMLCOMMENT_H
#define INLINEHTMLCOMMENT_H

#include "../TextHolder.h"

class InlineHtmlComment : public TextHolder {
    public:
    InlineHtmlComment(const string& contents);

    virtual void writeToken(ostream& out) const;
};

#endif
