#ifndef ORDEREDLIST_H
#define ORDEREDLIST_H

#include "UnorderedList.h"

namespace markdown {
    class OrderedList: public UnorderedList {
        public:
        OrderedList(const TokenGroup& contents, bool paragraphMode = false);
        virtual TokenPtr clone(const TokenGroup& newContents) const;
        virtual string containerName() const;

        protected:
        virtual void preWrite(ostream& out) const;
        virtual void postWrite(ostream& out) const;
    };
}

#endif
