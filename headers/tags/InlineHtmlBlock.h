#ifndef INLINEHTMLBLOCK_H
#define INLINEHTMLBLOCK_H

#include <string>
#include "../Container.h"

using namespace std;

class InlineHtmlBlock : public Container {
    public:
    InlineHtmlBlock(const TokenGroup& contents, bool isBlockTag = false);
    InlineHtmlBlock(const string& contents);

    virtual bool inhibitParagraphs() const;
    virtual TokenPtr clone(const TokenGroup& newContents) const;
    virtual string container_name() const;
    // Inline HTML blocks always end with a blank line, so report it as one for parsing purposes
    virtual bool is_blanck_line() const;

    private:
    bool mIsBlockTag;
};

#endif
