#ifndef RAWTEXT_H
#define RAWTEXT_H

#include <boost/lexical_cast.hpp>
#include <boost/unordered_set.hpp>
#include <string>
#include "../TextHolder.h"

using namespace std;

boost::unordered_set<string> otherTags, blockTags;

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

   void initTag(boost::unordered_set<string> &set, const char *init[]) {
        for (size_t i = 0; init[i] != 0; i++) {
            string str = init[i];

            if (*str.rbegin() == '/') {
                // Means it can have a closing tag
                str = str.substr(0, str.length() - 1);
            }

            set.insert(str);
        }
    }

    /**
     *
     */
    const char *other_tags[] = {
        "title/", "link", "script/", "style/", "meta", "em/",
        "strong/", "q/", "cite/", "a/", "img", "br", "span/",
        0
    };

    const char *cBlockTagInit[]= {
        "p/", "blockquote/", "hr", "h1/", "h2/", "h3/", "h4/", "h5/", "h6/",
        "dl/", "dt/", "dd/", "ol/", "ul/", "li/", "table/", "tr/", "th/",
        "td/", "thead/", "tbody/", "tfoot/", "form/", "select/", "option",
        "input", "label/", "textarea/", "div/", "pre/", "address/", "b/", "i/",
        0
    };

    bool notValidSiteCharacter(char c) {
        // NOTE: Kludge alert! The official spec for site characters is only
        // "a-zA-Z._%-". However, MDTest supports "international domain names,"
        // which use characters other than that; I'm kind of cheating here, handling
        // those by allowing all utf8-encoded characters too.
        return !(isalnum(c) || c == '.' || c == '_' || c == '%' || c == '-' || (c & 0x80));
    }

    bool notValidNameCharacter(char c) {
        return !(isalnum(c) || c == '.' || c == '_' || c == '%' || c == '-' || c == '+');
    }

    bool isNotAlpha(char c) {
        return !isalpha(c);
    }

    string emailEncode(const string& src) {
        ostringstream out;

        bool inHex = false;

        for (string::const_iterator i = src.begin(), ie = src.end(); i != ie; ++i) {
            if (*i & 0x80){
                out << *i;
            }
            else if (inHex) {
                out << "&#x" << hex << static_cast<int>(*i) << ';';
            }
            else {
                out << "&#" << dec << static_cast<int>(*i) << ';';
            }

            inHex=!inHex;
        }

        return out.str();
    }

    size_t isValidTag(const string& tag, bool nonBlockFirst = false) {
        if (blockTags.empty()) {
            initTag(otherTags, other_tags);
            initTag(blockTags, cBlockTagInit);
        }

        if (nonBlockFirst) {
            if (otherTags.find(tag) != otherTags.end()) return 1;
            if (blockTags.find(tag) != blockTags.end()) return 2;
        }
        else {
            if (blockTags.find(tag) != blockTags.end()) return 2;
            if (otherTags.find(tag) != otherTags.end()) return 1;
        }

        return 0;
    }

    string cleanTextLinkRef(const string& ref) {
        string r;

        for (string::const_iterator i = ref.begin(), ie = ref.end(); i != ie; ++i) {
            if (*i == ' ') {
                if (r.empty() || *r.rbegin() != ' ') {
                    r.push_back(' ');
                }
            }
            else {
                r.push_back(*i);
            }
        }

        return r;
    }

    /**
     * Défini si le paramètre a le format d'une URL
     */
    bool match_url(const string& str) {
        const char *patterns[] = { "http://", "https://",  "www.", 0 };

        for (int i = 0; patterns[i] != 0; i++) {
            const char *s = str.c_str();
            const char *t = patterns[i];

            while (*s != 0 && *t != 0 && *s == *t) {
                s++;
                t++;
            }

            if (*t == 0) {
                return true;
            }
        }

        return false;
    }

    /**
     * Défini si le paramètre a le format d'une adresse email
     */
    bool match_email_address(const string& str) {
        typedef string::const_iterator Iterator;
        typedef string::const_reverse_iterator Reverse_iterator;

        Iterator i = find_if(str.begin(), str.end(), notValidNameCharacter);

        if (i != str.end() && *i == '@' && i != str.begin()) {
            // The name part is valid.
            i = find_if(i + 1, str.end(), notValidSiteCharacter);

            if (i == str.end()) {
                // The site part doesn't contain any invalid characters.
                Reverse_iterator ri = find_if(str.rbegin(), str.rend(), isNotAlpha);

                if (ri != str.rend() && *ri == '.') {
                    // It ends with a dot and only alphabetic characters.
                    size_t d = distance(ri.base(), str.end());

                    if (d >= 2 && d <= 4) {
                        // There are two-to-four of them. It's valid.
                        return true;
                    }
                }
            }
        }

        return false;
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
