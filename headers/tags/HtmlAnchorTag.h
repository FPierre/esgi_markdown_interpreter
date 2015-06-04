#ifndef HTMLANCHORTAG_H
#define HTMLANCHORTAG_H

#include "../TextHolder.h"

class HtmlAnchorTag : public TextHolder {
    public:
    HtmlAnchorTag(const string& url, const string& title = string());

    virtual void write_token(ostream& out) const;
};

#endif
