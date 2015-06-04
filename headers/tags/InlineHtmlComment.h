#ifndef INLINEHTMLCOMMENT_H
#define INLINEHTMLCOMMENT_H

#include "../TextHolder.h"

class InlineHtmlComment : public TextHolder {
    public:
    InlineHtmlComment(const string& contents);

    virtual void write_token(ostream& out) const;
};

#endif
