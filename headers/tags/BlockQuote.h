#ifndef BLOCKQUOTE_H
#define BLOCKQUOTE_H

#include "../Container.h"

class BlockQuote : public Container {
    public:
    BlockQuote(const TokenGroup& contents);

    virtual TokenPtr clone(const TokenGroup& newContents) const;
    virtual string containerName() const;

    protected:
    virtual void preWrite(ostream& out) const;
    virtual void postWrite(ostream& out) const;
};

#endif
