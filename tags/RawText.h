#ifndef RAWTEXT_H
#define RAWTEXT_H

#include <string>
#include "../TextHolder.h"

using namespace std;

namespace markdown {
    const string cEscapedCharacters("\\`*_{}[]()#+-.!>");

    optional<size_t> isEscapedCharacter(char c) {
        string::const_iterator i = find(cEscapedCharacters.begin(), cEscapedCharacters.end(), c);

        if (i != cEscapedCharacters.end()) {
            return distance(cEscapedCharacters.begin(), i);
        }
        else {
            return none;
        }
    }

    char escapedCharacter(size_t index) {
        return cEscapedCharacters[index];
    }

    class RawText : public TextHolder {
        public:
        RawText(const string& text, bool canContainMarkup = true);
        virtual void writeToken(ostream& out) const;
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
}

#endif
