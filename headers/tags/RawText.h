#ifndef RAWTEXT_H
#define RAWTEXT_H

#include <boost/lexical_cast.hpp>
#include <boost/unordered_set.hpp>
#include <string>
#include "../TextHolder.h"

using namespace std;

size_t isValidTag(const string& tag, bool nonBlockFirst = false);

class RawText : public TextHolder {
    public:
    RawText(const string& text, bool canContainMarkup = true);

    virtual void write_token(ostream& out) const;
    virtual optional<TokenGroup> process_span_elements(const LinkIds& idTable);

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

#endif
