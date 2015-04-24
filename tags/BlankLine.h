#ifndef BLANKLINE_H
#define BLANKLINE_H

#include "../TextHolder.h"

namespace markdown {
    class BlankLine : public TextHolder {
        public:
        BlankLine(const string& actualContents = string());
        virtual void writeToken(ostream& out) const;
        virtual bool isBlankLine() const;
    };
}

#endif
