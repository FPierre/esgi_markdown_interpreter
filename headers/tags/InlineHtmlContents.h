#ifndef INLINEHTMLCONTENTS_H
#define INLINEHTMLCONTENTS_H

#include "../TextHolder.h"

class InlineHtmlContents : public TextHolder {
    public:
    InlineHtmlContents(const string& contents);
    virtual void writeToken(ostream& out) const;
};

#endif
