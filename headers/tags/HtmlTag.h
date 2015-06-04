#ifndef HTMLTAG_H
#define HTMLTAG_H

#include "../TextHolder.h"

class HtmlTag : public TextHolder {
    public:
    HtmlTag(const string& contents);

    virtual void write_token(ostream& out) const;

    protected:
    virtual void pre_write(ostream& out) const;
    virtual void post_write(ostream& out) const;
};

#endif
