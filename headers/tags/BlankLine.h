#ifndef BLANKLINE_H
#define BLANKLINE_H

#include "../TextHolder.h"

class BlankLine : public TextHolder {
    public:
    BlankLine(const string& actualContents = string());

    virtual void write_token(ostream& out) const;
    virtual bool is_blank_line() const;
};

#endif
