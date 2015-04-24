#include <string>
#include <boost/regex.hpp>
#include "TextHolder.h"
#include "Token.h"

using namespace std;

namespace markdown {
    TextHolder::TextHolder(const string& text, bool canContainMarkup, unsigned int encodingFlags) : mText(text),
                                                                                                    mCanContainMarkup(canContainMarkup),
                                                                                                    mEncodingFlags(encodingFlags) {

    }

    string TextHolder::encodeString(const string& src, int encodingFlags) {
        bool amps      = (encodingFlags & cAmps)       != 0,
        doubleAmps     = (encodingFlags & cDoubleAmps) != 0,
        angleBrackets  = (encodingFlags & cAngles)     != 0,
        quotes         = (encodingFlags & cQuotes)     != 0;

        string tgt;

        for (string::const_iterator i = src.begin(), ie = src.end(); i != ie; ++i) {
            if (*i == '&' && amps) {
                static const boost::regex cIgnore("^(&amp;)|(&#[0-9]{1,3};)|(&#[xX][0-9a-fA-F]{1,2};)");

                if (boost::regex_search(i, ie, cIgnore)) {
                    tgt.push_back(*i);
                }
                else {
                    tgt += "&amp;";
                }
            }
            else if (*i == '&'  && doubleAmps)    tgt += "&amp;";
            else if (*i == '<'  && angleBrackets) tgt += "&lt;";
            else if (*i == '>'  && angleBrackets) tgt += "&gt;";
            else if (*i == '\"' && quotes)        tgt += "&quot;";
            else tgt.push_back(*i);
        }

        return tgt;
    }

    void TextHolder::interprete_to_html(ostream& out) const {
        preWrite(out);

        if (mEncodingFlags != 0) {
            // TODO: erreur si décommente cette ligne...
            // out << encodeString(mText, mEncodingFlags);
        }
        else {
            out << mText;
        }

        postWrite(out);
    }

    void TextHolder::writeToken(ostream& out) const {
        out << "TextHolder: " << mText << '\n';
    }

    optional<const string&> TextHolder::text() const {
        return mText;
    }

    bool TextHolder::canContainMarkup() const {
        return mCanContainMarkup;
    }
}
