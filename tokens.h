#ifndef TOKENS_H
#define TOKENS_H

#include <vector>
#include "Document.h"

namespace markdown {

typedef TokenGroup::iterator TokenGroupIter;
typedef TokenGroup::const_iterator CTokenGroupIter;

class LinkIds {
    public:
    struct Target {
        string url;
        string title;

        Target(const string& url_, const string& title_):
            url(url_), title(title_) { }
    };

    optional<Target> find(const string& id) const;
    void add(const string& id, const string& url, const
        string& title);

    private:
    typedef boost::unordered_map<string, Target> Table;

    static string _scrubKey(string str);

    Table mTable;
};

class Token {
    public:
    Token() { }

    virtual void interprete_to_html(ostream&) const=0;
    virtual void writeAsOriginal(ostream& out) const { interprete_to_html(out); }
    virtual void writeToken(ostream& out) const=0;
    virtual void writeToken(size_t indent, ostream& out) const {
        out << string(indent*2, ' ');
        writeToken(out);
    }

    virtual optional<TokenGroup> processSpanElements(const LinkIds& idTable)
        { return none; }

    virtual optional<const string&> text() const { return none; }

    virtual bool canContainMarkup() const { return false; }
    virtual bool isBlankLine() const { return false; }
    virtual bool isContainer() const { return false; }
    virtual bool isUnmatchedOpenMarker() const { return false; }
    virtual bool isUnmatchedCloseMarker() const { return false; }
    virtual bool isMatchedOpenMarker() const { return false; }
    virtual bool isMatchedCloseMarker() const { return false; }
    virtual bool inhibitParagraphs() const { return false; }

    protected:
    virtual void preWrite(ostream& out) const { }
    virtual void postWrite(ostream& out) const { }
};



size_t isValidTag(const string& tag, bool nonBlockFirst=false);

enum EncodingFlags { cAmps=0x01, cDoubleAmps=0x02, cAngles=0x04, cQuotes=0x08 };

class TextHolder: public Token {
    public:
    TextHolder(const string& text, bool canContainMarkup, unsigned int
        encodingFlags): mText(text), mCanContainMarkup(canContainMarkup),
        mEncodingFlags(encodingFlags) { }

    virtual void interprete_to_html(ostream& out) const;

    virtual void writeToken(ostream& out) const { out << "TextHolder: " << mText << '\n'; }

    virtual optional<const string&> text() const { return mText; }

    virtual bool canContainMarkup() const { return mCanContainMarkup; }

    private:
    const string mText;
    const bool mCanContainMarkup;
    const int mEncodingFlags;
};

class RawText: public TextHolder {
    public:
    RawText(const string& text, bool canContainMarkup=true):
        TextHolder(text, canContainMarkup, cAmps|cAngles|cQuotes) { }

    virtual void writeToken(ostream& out) const { out << "RawText: " << *text() << '\n'; }

    virtual optional<TokenGroup> processSpanElements(const LinkIds& idTable);

    private:
    typedef vector<TokenPtr> ReplacementTable;

    static string _processHtmlTagAttributes(string src, ReplacementTable& replacements);
    static string _processCodeSpans(string src, ReplacementTable& replacements);
    static string _processEscapedCharacters(const string& src);
    static string _processLinksImagesAndTags(const string& src, ReplacementTable& replacements, const LinkIds& idTable);
    static string _processSpaceBracketedGroupings(const string& src, ReplacementTable& replacements);
    static TokenGroup _processBoldAndItalicSpans(const string& src, ReplacementTable& replacements);

    static TokenGroup _encodeProcessedItems(const string& src, ReplacementTable& replacements);
    static string _restoreProcessedItems(const string &src, ReplacementTable& replacements);
};

class HtmlTag: public TextHolder {
    public:
    HtmlTag(const string& contents): TextHolder(contents, false, cAmps|cAngles) { }

    virtual void writeToken(ostream& out) const { out << "HtmlTag: " << *text() << '\n'; }

    protected:
    virtual void preWrite(ostream& out) const { out << '<'; }
    virtual void postWrite(ostream& out) const { out << '>'; }
};

class HtmlAnchorTag: public TextHolder {
    public:
    HtmlAnchorTag(const string& url, const string& title=string());

    virtual void writeToken(ostream& out) const { out << "HtmlAnchorTag: " << *text() << '\n'; }
};

class InlineHtmlContents: public TextHolder {
    public:
    InlineHtmlContents(const string& contents): TextHolder(contents, false,
        cAmps|cAngles) { }

    virtual void writeToken(ostream& out) const { out << "InlineHtmlContents: " << *text() << '\n'; }
};

class InlineHtmlComment: public TextHolder {
    public:
    InlineHtmlComment(const string& contents): TextHolder(contents, false,
        0) { }

    virtual void writeToken(ostream& out) const { out << "InlineHtmlComment: " << *text() << '\n'; }
};

class CodeBlock: public TextHolder {
    public:
    CodeBlock(const string& actualContents): TextHolder(actualContents,
        false, cDoubleAmps|cAngles|cQuotes) { }

    virtual void interprete_to_html(ostream& out) const;

    virtual void writeToken(ostream& out) const { out << "CodeBlock: " << *text() << '\n'; }
};

class CodeSpan: public TextHolder {
    public:
    CodeSpan(const string& actualContents): TextHolder(actualContents,
        false, cDoubleAmps|cAngles|cQuotes) { }

    virtual void interprete_to_html(ostream& out) const;
    virtual void writeAsOriginal(ostream& out) const;
    virtual void writeToken(ostream& out) const { out << "CodeSpan: " << *text() << '\n'; }
};

class Header: public TextHolder {
    public:
    Header(size_t level, const string& text): TextHolder(text, true,
        cAmps|cAngles|cQuotes), mLevel(level) { }

    virtual void writeToken(ostream& out) const { out << "Header " <<
        mLevel << ": " << *text() << '\n'; }

    virtual bool inhibitParagraphs() const { return true; }

    protected:
    virtual void preWrite(ostream& out) const { out << "<h" << mLevel << ">"; }
    virtual void postWrite(ostream& out) const { out << "</h" << mLevel << ">\n"; }

    private:
    size_t mLevel;
};

class BlankLine: public TextHolder {
    public:
    BlankLine(const string& actualContents=string()):
        TextHolder(actualContents, false, 0) { }

    virtual void writeToken(ostream& out) const { out << "BlankLine: " << *text() << '\n'; }

    virtual bool isBlankLine() const { return true; }
};

class EscapedCharacter: public Token {
    public:
    EscapedCharacter(char c): mChar(c) { }

    virtual void interprete_to_html(ostream& out) const { out << mChar; }
    virtual void writeAsOriginal(ostream& out) const { out << '\\' << mChar; }
    virtual void writeToken(ostream& out) const { out << "EscapedCharacter: " << mChar << '\n'; }

    private:
    const char mChar;
};

class Container: public Token {
    public:
    Container(const TokenGroup& contents=TokenGroup()): mSubTokens(contents),
        mParagraphMode(false) { }

    const TokenGroup& subTokens() const { return mSubTokens; }
    void appendSubtokens(TokenGroup& tokens) { mSubTokens.splice(mSubTokens.end(), tokens); }
    void swapSubtokens(TokenGroup& tokens) { mSubTokens.swap(tokens); }

    virtual bool isContainer() const { return true; }

    virtual void interprete_to_html(ostream& out) const;

    virtual void writeToken(ostream& out) const { out << "Container: error!" << '\n'; }
    virtual void writeToken(size_t indent, ostream& out) const;

    virtual optional<TokenGroup> processSpanElements(const LinkIds& idTable);

    virtual TokenPtr clone(const TokenGroup& newContents) const { return TokenPtr(new Container(newContents)); }
    virtual string containerName() const { return "Container"; }

    protected:
    TokenGroup mSubTokens;
    bool mParagraphMode;
};

class InlineHtmlBlock: public Container {
    public:
    InlineHtmlBlock(const TokenGroup& contents, bool isBlockTag=false):
        Container(contents), mIsBlockTag(isBlockTag) { }
    InlineHtmlBlock(const string& contents): mIsBlockTag(false) {
        mSubTokens.push_back(TokenPtr(new InlineHtmlContents(contents)));
    }

    virtual bool inhibitParagraphs() const { return !mIsBlockTag; }
    virtual TokenPtr clone(const TokenGroup& newContents) const { return TokenPtr(new InlineHtmlBlock(newContents)); }
    virtual string containerName() const { return "InlineHtmlBlock"; }
    // Inline HTML blocks always end with a blank line, so report it as one for
    // parsing purposes.
    virtual bool isBlankLine() const { return true; }

    private:
    bool mIsBlockTag;
};

class ListItem: public Container {
    public:
    ListItem(const TokenGroup& contents): Container(contents), mInhibitParagraphs(true) { }
    void inhibitParagraphs(bool set) { mInhibitParagraphs=set; }
    virtual bool inhibitParagraphs() const { return mInhibitParagraphs; }
    virtual TokenPtr clone(const TokenGroup& newContents) const { return TokenPtr(new ListItem(newContents)); }
    virtual string containerName() const { return "ListItem"; }

    protected:
    virtual void preWrite(ostream& out) const { out << "<li>"; }
    virtual void postWrite(ostream& out) const { out << "</li>\n"; }

    private:
    bool mInhibitParagraphs;
};

class UnorderedList: public Container {
    public:
    UnorderedList(const TokenGroup& contents, bool paragraphMode=false);
    virtual TokenPtr clone(const TokenGroup& newContents) const { return TokenPtr(new UnorderedList(newContents)); }
    virtual string containerName() const { return "UnorderedList"; }

    protected:
    virtual void preWrite(ostream& out) const { out << "\n<ul>\n"; }
    virtual void postWrite(ostream& out) const { out << "</ul>\n\n"; }
};

class OrderedList: public UnorderedList {
    public:
    OrderedList(const TokenGroup& contents, bool paragraphMode=false) : UnorderedList(contents, paragraphMode) { }
    virtual TokenPtr clone(const TokenGroup& newContents) const { return TokenPtr(new OrderedList(newContents)); }
    virtual string containerName() const { return "OrderedList"; }

    protected:
    virtual void preWrite(ostream& out) const { out << "<ol>\n"; }
    virtual void postWrite(ostream& out) const { out << "</ol>\n\n"; }
};

class BlockQuote: public Container {
    public:
    BlockQuote(const TokenGroup& contents): Container(contents) { }
    virtual TokenPtr clone(const TokenGroup& newContents) const { return TokenPtr(new BlockQuote(newContents)); }
    virtual string containerName() const { return "BlockQuote"; }

    protected:
    virtual void preWrite(ostream& out) const { out << "<blockquote>\n"; }
    virtual void postWrite(ostream& out) const { out << "\n</blockquote>\n"; }
};

class Paragraph: public Container {
    public:
    Paragraph() { }
    Paragraph(const TokenGroup& contents): Container(contents) { }
    virtual TokenPtr clone(const TokenGroup& newContents) const { return TokenPtr(new Paragraph(newContents)); }
    virtual string containerName() const { return "Paragraph"; }

    protected:
    virtual void preWrite(ostream& out) const { out << "<p>"; }
    virtual void postWrite(ostream& out) const { out << "</p>\n\n"; }
};

class BoldOrItalicMarker: public Token {
    public:
    BoldOrItalicMarker(bool open, char c, size_t size): mOpenMarker(open),
        mTokenCharacter(c), mSize(size), mMatch(0), mCannotMatch(false),
        mDisabled(false), mId(-1) { }

    virtual bool isUnmatchedOpenMarker() const { return (mOpenMarker && mMatch==0 && !mCannotMatch); }
    virtual bool isUnmatchedCloseMarker() const { return (!mOpenMarker && mMatch==0 && !mCannotMatch); }
    virtual bool isMatchedOpenMarker() const { return (mOpenMarker && mMatch!=0); }
    virtual bool isMatchedCloseMarker() const { return (!mOpenMarker && mMatch!=0); }
    virtual void interprete_to_html(ostream& out) const;
    virtual void writeToken(ostream& out) const;

    bool isOpenMarker() const { return mOpenMarker; }
    char tokenCharacter() const { return mTokenCharacter; }
    size_t size() const { return mSize; }
    bool matched() const { return (mMatch!=0); }
    BoldOrItalicMarker* matchedTo() const { return mMatch; }
    int id() const { return mId; }

    void matched(BoldOrItalicMarker *match, int id=-1) { mMatch=match; mId=id; }
    void cannotMatch(bool set) { mCannotMatch=set; }
    void disable() { mCannotMatch=mDisabled=true; }

    private:
    bool mOpenMarker; // Otherwise it's a close-marker
    char mTokenCharacter; // Underscore or asterisk
    size_t mSize; // 1=italics, 2=bold, 3=both
    BoldOrItalicMarker* mMatch;
    bool mCannotMatch;
    bool mDisabled;
    int mId;
};

class Image: public Token {
    public:
    Image(const string& altText, const string& url, const string& title): mAltText(altText), mUrl(url), mTitle(title) { }
    virtual void interprete_to_html(ostream& out) const;
    virtual void writeToken(ostream& out) const { out << "Image: " << mUrl << '\n'; }

    private:
    const string mAltText, mUrl, mTitle;
};


} // namespace markdown

#endif
