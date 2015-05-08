#include "../../headers/tags/InlineHtmlBlock.h"

using namespace std;


    InlineHtmlBlock::InlineHtmlBlock(const TokenGroup& contents, bool isBlockTag) : Container(contents),
                                                                                    mIsBlockTag(isBlockTag) {

    }

    InlineHtmlBlock::InlineHtmlBlock(const string& contents) : mIsBlockTag(false) {
        // TODO: bug si décommenté
        // mSubTokens.push_back(TokenPtr(new InlineHtmlContents(contents)));
    }

    bool InlineHtmlBlock::inhibitParagraphs() const {
        return !mIsBlockTag;
    }

    TokenPtr InlineHtmlBlock::clone(const TokenGroup& newContents) const {
        return TokenPtr(new InlineHtmlBlock(newContents));
    }

    string InlineHtmlBlock::containerName() const {
        return "InlineHtmlBlock";
    }

    // Inline HTML ocks always end with a blank line, so report it as one for parsing purposes
    bool InlineHtmlBlock::isBlankLine() const {
        return true;
    }

