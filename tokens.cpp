#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/unordered_set.hpp>
#include <stack>
#include "tokens.h"

using namespace std;

namespace markdown {

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

bool notValidNameCharacter(char c) {
    return !(isalnum(c) || c == '.' || c == '_' || c == '%' || c == '-' || c == '+');
}

bool notValidSiteCharacter(char c) {
    // NOTE: Kludge alert! The official spec for site characters is only
    // "a-zA-Z._%-". However, MDTest supports "international domain names,"
    // which use characters other than that; I'm kind of cheating here, handling
    // those by allowing all utf8-encoded characters too.
    return !(isalnum(c) || c == '.' || c == '_' || c == '%' || c == '-' || (c & 0x80));
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

// Other official ones (not presently in use in this code)
//"!doctype", "bdo", "body", "button", "fieldset", "head", "html",
//"legend", "noscript", "optgroup", "xmp",

boost::unordered_set<string> otherTags, blockTags;

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

size_t isValidTag(const string& tag, bool nonBlockFirst) {
    if (blockTags.empty()) {
        initTag(otherTags, other_tags);
        initTag(blockTags, cBlockTagInit);
    }

    if (nonBlockFirst) {
        if (otherTags.find(tag)!=otherTags.end()) return 1;
        if (blockTags.find(tag)!=blockTags.end()) return 2;
    }
    else {
        if (blockTags.find(tag)!=blockTags.end()) return 2;
        if (otherTags.find(tag)!=otherTags.end()) return 1;
    }

    return 0;
}

void BoldOrItalicMarker::interprete_to_html(ostream& out) const {
    if (!mDisabled) {
        if (mMatch != 0) {
            assert(mSize >= 1 && mSize <= 3);

            if (mOpenMarker) {
                out << (mSize == 1 ? "<em>" : mSize == 2 ? "<strong>" : "<strong><em>");
            }
            else {
                out << (mSize == 1 ? "</em>" : mSize == 2 ? "</strong>" : "</em></strong>");
            }
        }
        else {
            out << string(mSize, mTokenCharacter);
        }
    }
}

void BoldOrItalicMarker::writeToken(ostream& out) const {
    if (!mDisabled) {
        if (mMatch != 0) {
            string type = (mSize == 1 ? "italic" : mSize == 2 ? "bold" : "italic&bold");

            if (mOpenMarker) {
                out << "Matched open-" << type << " marker" << endl;
            }
            else {
                out << "Matched close-" << type << " marker" << endl;
            }
        }
        else {
            if (mOpenMarker) {
                out << "Unmatched bold/italic open marker: " << string(mSize, mTokenCharacter) << endl;
            }
            else {
                out << "Unmatched bold/italic close marker: " << string(mSize, mTokenCharacter) << endl;
            }
        }
    }
}

void Image::interprete_to_html(ostream& out) const {
    out << "<img src=\"" << mUrl << "\" alt=\"" << mAltText << "\"";

    if (!mTitle.empty()) {
        out << " title=\"" << mTitle << "\"";
    }

    out << "/>";
}


}
