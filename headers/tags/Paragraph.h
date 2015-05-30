#ifndef PARAGRAPH_H
#define PARAGRAPH_H

#include "../Container.h"

class Paragraph : public Container {
    public:
    Paragraph();
    Paragraph(const TokenGroup& contents);

    virtual TokenPtr clone(const TokenGroup& newContents) const;
    virtual string containerName() const;

    protected:
    virtual void preWrite(ostream& out) const;
    virtual void postWrite(ostream& out) const;
};

#endif
