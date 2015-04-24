#ifndef HTMLTAG_H
#define HTMLTAG_H

#include "../TextHolder.h"

namespace markdown {
    class HtmlTag : public TextHolder {
        public:
        HtmlTag(const string& contents);
        virtual void writeToken(ostream& out) const;

        protected:
        virtual void preWrite(ostream& out) const;
        virtual void postWrite(ostream& out) const;
    };
}

#endif
