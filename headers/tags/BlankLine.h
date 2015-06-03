#ifndef BLANKLINE_H
#define BLANKLINE_H

#include "../TextHolder.h"

class BlankLine : public TextHolder {
    public:
    BlankLine(const string& actualContents = string());

    virtual void writeToken(ostream& out) const;
    virtual bool is_blanck_line() const;
};

#endif
