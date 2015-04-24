#ifndef HEADER_H
#define HEADER_H

#include "../TextHolder.h"

namespace markdown {
    class Header : public TextHolder {
        public:
        Header(size_t level, const string& text);
        virtual void writeToken(ostream& out) const;
        virtual bool inhibitParagraphs() const;

        protected:
        virtual void preWrite(ostream& out) const;
        virtual void postWrite(ostream& out) const;

        private:
        size_t mLevel;
    };
}

#endif
