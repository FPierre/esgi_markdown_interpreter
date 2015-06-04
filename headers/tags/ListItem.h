#ifndef LISTITEM_H
#define LISTITEM_H

#include "../Container.h"

class ListItem : public Container {
    public:
    ListItem(const TokenGroup& contents);

    void inhibitParagraphs(bool set);
    virtual bool inhibitParagraphs() const;
    virtual TokenPtr clone(const TokenGroup& newContents) const;
    virtual string containerName() const;

    protected:
    virtual void pre_write(ostream& out) const;
    virtual void post_write(ostream& out) const;

    private:
    bool mInhibitParagraphs;
};

#endif
