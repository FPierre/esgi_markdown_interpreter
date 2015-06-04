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
    virtual void pre_write(ostream& out) const;
    virtual void post_write(ostream& out) const;
};

#endif
