#include <boost/regex.hpp>
#include "RawText.h"

using namespace std;

namespace markdown {
    RawText::RawText(const string& text, bool canContainMarkup) : TextHolder(text, canContainMarkup, cAmps|cAngles|cQuotes) {

    }

    void RawText::writeToken(ostream& out) const {
        out << "RawText: " << *text() << '\n';
    }

    optional<TokenGroup> RawText::processSpanElements(const LinkIds& idTable) {
        if (!canContainMarkup()) {
            return none;
        }

        ReplacementTable replacements;

        string str= _processHtmlTagAttributes(*text(), replacements);
        str       = _processCodeSpans(str, replacements);
        str       = _processEscapedCharacters(str);
        str       = _processLinksImagesAndTags(str, replacements, idTable);

        return _processBoldAndItalicSpans(str, replacements);
    }

    typedef vector<TokenPtr> ReplacementTable;

    static string RawText::_processHtmlTagAttributes(string src, ReplacementTable& replacements) {
        // Because "Attribute Content Is Not A Code Span"
        string tgt;
        string::const_iterator prev = src.begin(), end = src.end();

        while (1) {
            static const boost::regex cHtmlToken("<((/?)([a-zA-Z0-9]+)(?:( +[a-zA-Z0-9]+?(?: ?= ?(\"|').*?\\5))+? */? *))>");
            boost::smatch m;

            if (boost::regex_search(prev, end, m, cHtmlToken)) {
                // NOTE: Kludge alert! The `isValidTag` test is a cheat, only here
                // to handle some edge cases between the Markdown test suite and the
                // PHP-Markdown one, which seem to conflict.
                if (isValidTag(m[3])) {
                    tgt += string(prev, m[0].first);

                    string fulltag = m[0], tgttag;
                    string::const_iterator prevtag = fulltag.begin(), endtag = fulltag.end();

                    while (1) {
                        static const boost::regex cAttributeStrings("= ?(\"|').*?\\1");
                        boost::smatch mtag;

                        if (boost::regex_search(prevtag, endtag, mtag, cAttributeStrings)) {
                            tgttag += string(prevtag, mtag[0].first);
                            tgttag += "\x01@" + boost::lexical_cast<string>(replacements.size()) + "@htmlTagAttr\x01";
                            prevtag = mtag[0].second;

                            replacements.push_back(TokenPtr(new TextHolder(string(mtag[0]), false, cAmps|cAngles)));
                        }
                        else {
                            tgttag += string(prevtag, endtag);
                            break;
                        }
                    }

                    tgt += tgttag;
                    prev = m[0].second;
                }
                else {
                    tgt += string(prev, m[0].second);
                    prev = m[0].second;
                }
            }
            else {
                tgt += string(prev, end);
                break;
            }
        }

        return tgt;
    }

    static string RawText::_processCodeSpans(string src, ReplacementTable& replacements) {
        static const boost::regex cCodeSpan[2]={
            boost::regex("(?:^|(?<=[^\\\\]))`` (.+?) ``"),
            boost::regex("(?:^|(?<=[^\\\\]))`(.+?)`")
        };

        for (int pass = 0; pass < 2; pass++) {
            string tgt;
            string::const_iterator prev = src.begin(), end = src.end();

            while (1) {
                boost::smatch m;

                if (boost::regex_search(prev, end, m, cCodeSpan[pass])) {
                    tgt += string(prev, m[0].first);
                    tgt += "\x01@" + boost::lexical_cast<string>(replacements.size()) + "@codeSpan\x01";
                    prev = m[0].second;

                    replacements.push_back(TokenPtr(new CodeSpan(_restoreProcessedItems(m[1], replacements))));
                }
                else {
                    tgt += string(prev, end);
                    break;
                }
            }

            src.swap(tgt);
            tgt.clear();
        }

        return src;
    }

    static string RawText::_processEscapedCharacters(const string& src) {
        string tgt;
        string::const_iterator prev=src.begin(), end=src.end();

        while (1) {
            string::const_iterator i=find(prev, end, '\\');

            if (i != end) {
                tgt += string(prev, i);
                ++i;

                if (i != end) {
                    optional<size_t> e = isEscapedCharacter(*i);

                    if (e) {
                        tgt += "\x01@#" + boost::lexical_cast<string>(*e) + "@escaped\x01";
                    }
                    else {
                        tgt = tgt + '\\' + *i;
                    }

                    prev = i + 1;
                }
                else {
                    tgt += '\\';
                    break;
                }
            }
            else {
                tgt += string(prev, end);
                break;
            }
        }

        return tgt;
    }

    static string RawText::_processLinksImagesAndTags(const string& src, ReplacementTable& replacements, const LinkIds& idTable) {
        // NOTE: Kludge alert! The "inline link or image" regex should be...
        //
        //   "(?:(!?)\\[(.+?)\\] *\\((.*?)\\))"
        //
        // ...but that fails on the 'Images' test because it includes a "stupid URL"
        // that has parentheses within it. The proper way to deal with this would be
        // to match any nested parentheses, but regular expressions can't handle an
        // unknown number of nested items, so I'm cheating -- the regex for it
        // allows for one (and *only* one) pair of matched parentheses within the
        // URL. It makes the regex hard to follow (it was even harder to get right),
        // but it allows it to pass the test.
        //
        // The "reference link or image" one has a similar problem; it should be...
        //
        //   "|(?:(!?)\\[(.+?)\\](?: *\\[(.*?)\\])?)"
        //
        static const boost::regex cExpression(
            "(?:(!?)\\[([^\\]]+?)\\] *\\(([^\\(]*(?:\\(.*?\\).*?)*?)\\))" // Inline link or image
            "|(?:(!?)\\[((?:[^]]*?\\[.*?\\].*?)|(?:.+?))\\](?: *\\[(.*?)\\])?)" // Reference link or image
            "|(?:<(/?([a-zA-Z0-9]+).*?)>)" // potential HTML tag or auto-link
        );
        // Important captures: 1/4=image indicator, 2/5=contents/alttext,
        // 3=URL/title, 6=optional link ID, 7=potential HTML tag or auto-link
        // contents, 8=actual tag from 7.

        string tgt;
        string::const_iterator prev = src.begin();
        string::const_iterator end  = src.end();

        while (1) {
            boost::smatch m;

            if (boost::regex_search(prev, end, m, cExpression)) {
                assert(m[0].matched);
                assert(m[0].length() != 0);

                tgt += string(prev, m[0].first);
                tgt += "\x01@" + boost::lexical_cast<string>(replacements.size()) + "@links&Images1\x01";
                prev = m[0].second;

                bool isImage     = false;
                bool isLink      = false;
                bool isReference = false;

                if (m[4].matched && m[4].length()) isImage=isReference=true;
                else if (m[1].matched && m[1].length()) isImage=true;
                else if (m[5].matched) isLink=isReference=true;
                else if (m[2].matched) isLink=true;

                if (isImage || isLink) {
                    string contentsOrAlttext, url, title;
                    bool resolved=false;

                    if (isReference) {
                        contentsOrAlttext=m[5];
                        string linkId=(m[6].matched ? string(m[6]) : string());

                        if (linkId.empty()) linkId=cleanTextLinkRef(contentsOrAlttext);

                        optional<markdown::LinkIds::Target> target=idTable.find(linkId);

                        if (target) { url = target->url; title = target->title; resolved = true; };
                    }
                    else {
                        static const boost::regex cReference("^<?([^ >]*)>?(?: *(?:('|\")(.*)\\2)|(?:\\((.*)\\)))? *$");
                        // Useful captures: 1=url, 3/4=title
                        contentsOrAlttext=m[2];
                        string urlAndTitle=m[3];
                        boost::smatch mm;
                        if (boost::regex_match(urlAndTitle, mm, cReference)) {
                            url=mm[1];
                            if (mm[3].matched) title=mm[3];
                            else if (mm[4].matched) title=mm[4];
                            resolved=true;
                        }
                    }

                    if (!resolved) {
                        // Just encode the first character as-is, and continue
                        // searching after it.
                        prev=m[0].first+1;
                        replacements.push_back(TokenPtr(new RawText(string(m[0].first, prev))));
                    }
                    else if (isImage) {
                        replacements.push_back(TokenPtr(new Image(contentsOrAlttext, url, title)));
                    }
                    else {
                        replacements.push_back(TokenPtr(new HtmlAnchorTag(url, title)));
                        tgt += contentsOrAlttext;
                        tgt += "\x01@"+boost::lexical_cast<string>(replacements.size())+"@links&Images2\x01";
                        replacements.push_back(TokenPtr(new HtmlTag("/a")));
                    }
                }
                else {
                    // Otherwise it's an HTML tag or auto-link.
                    string contents=m[7];

    //              cerr << "Evaluating potential HTML or auto-link: " << contents << endl;
    //              cerr << "m[8]=" << m[8] << endl;

                    if (match_url(contents)) {
                        TokenGroup subgroup;
                        subgroup.push_back(TokenPtr(new HtmlAnchorTag(contents)));
                        subgroup.push_back(TokenPtr(new RawText(contents, false)));
                        subgroup.push_back(TokenPtr(new HtmlTag("/a")));
                        replacements.push_back(TokenPtr(new Container(subgroup)));
                    }
                    else if (match_email_address(contents)) {
                        TokenGroup subgroup;
                        subgroup.push_back(TokenPtr(new HtmlAnchorTag(emailEncode("mailto:"+contents))));
                        subgroup.push_back(TokenPtr(new RawText(emailEncode(contents), false)));
                        subgroup.push_back(TokenPtr(new HtmlTag("/a")));
                        replacements.push_back(TokenPtr(new Container(subgroup)));
                    }
                    else if (isValidTag(m[8])) {
                        replacements.push_back(TokenPtr(new HtmlTag(_restoreProcessedItems(contents, replacements))));
                    }
                    else {
                        // Just encode it as-is
                        replacements.push_back(TokenPtr(new RawText(m[0])));
                    }
                }
            }
            else {
                tgt += string(prev, end);
                break;
            }
        }

        return tgt;
    }

    static string RawText::_processSpaceBracketedGroupings(const string& src, ReplacementTable& replacements) {
        static const boost::regex cRemove("(?:(?: \\*+ )|(?: _+ ))");

        string tgt;
        string::const_iterator prev = src.begin();
        string::const_iterator end  = src.end();

        while (1) {
            boost::smatch m;

            if (boost::regex_search(prev, end, m, cRemove)) {
                tgt += string(prev, m[0].first);
                tgt += "\x01@" + boost::lexical_cast<string>(replacements.size()) + "@spaceBracketed\x01";

                replacements.push_back(TokenPtr(new RawText(m[0])));

                prev = m[0].second;
            }
            else {
                tgt += string(prev, end);
                break;
            }
        }
        return tgt;
    }

    static TokenGroup RawText::_processBoldAndItalicSpans(const string& src, ReplacementTable& replacements) {
        static const boost::regex cEmphasisExpression(
            "(?:(?<![*_])([*_]{1,3})([^*_ ]+?)\\1(?![*_]))"                                    // Mid-word emphasis
            "|((?:(?<!\\*)\\*{1,3}(?!\\*)|(?<!_)_{1,3}(?!_))(?=.)(?! )(?![.,:;] )(?![.,:;]$))" // Open
            "|((?<![* ])\\*{1,3}(?!\\*)|(?<![ _])_{1,3}(?!_))"                                 // Close
        );

        TokenGroup tgt;
        string::const_iterator i=src.begin(), end=src.end(), prev=i;

        while (1) {
            boost::smatch m;

            if (boost::regex_search(prev, end, m, cEmphasisExpression)) {
                if (prev!=m[0].first) tgt.push_back(TokenPtr(new RawText(string(prev, m[0].first))));

                if (m[3].matched) {
                    string token=m[3];
                    tgt.push_back(TokenPtr(new BoldOrItalicMarker(true, token[0], token.length())));
                    prev=m[0].second;
                }
                else if (m[4].matched) {
                    string token=m[4];
                    tgt.push_back(TokenPtr(new BoldOrItalicMarker(false, token[0], token.length())));

                    prev = m[0].second;
                }
                else {
                    string token=m[1], contents=m[2];
                    tgt.push_back(TokenPtr(new BoldOrItalicMarker(true, token[0], token.length())));
                    tgt.push_back(TokenPtr(new RawText(string(contents))));
                    tgt.push_back(TokenPtr(new BoldOrItalicMarker(false, token[0], token.length())));

                    prev = m[0].second;
                }
            }
            else {
                if (prev!=end) tgt.push_back(TokenPtr(new RawText(string(prev, end))));
                break;
            }
        }

        int id = 0;

        for (TokenGroup::iterator ii=tgt.begin(), iie=tgt.end(); ii!=iie; ++ii) {
            if ((*ii)->isUnmatchedOpenMarker()) {
                BoldOrItalicMarker *openToken=dynamic_cast<BoldOrItalicMarker *>(ii->get());

                // Find a matching close-marker, if it's there
                TokenGroup::iterator iii=ii;

                for (++iii; iii!=iie; ++iii) {
                    if ((*iii)->isUnmatchedCloseMarker()) {
                        BoldOrItalicMarker *closeToken=dynamic_cast<BoldOrItalicMarker*>(iii->get());
                        if (closeToken->size()==3 && openToken->size()!=3) {
                            // Split the close-token into a match for the open-token
                            // and a second for the leftovers.
                            closeToken->disable();
                            TokenGroup g;
                            g.push_back(TokenPtr(new BoldOrItalicMarker(false,
                                closeToken->tokenCharacter(), closeToken->size()-
                                openToken->size())));
                            g.push_back(TokenPtr(new BoldOrItalicMarker(false,
                                closeToken->tokenCharacter(), openToken->size())));
                            TokenGroup::iterator after=iii;
                            ++after;
                            tgt.splice(after, g);
                            continue;
                        }

                        if (closeToken->tokenCharacter()==openToken->tokenCharacter() && closeToken->size()==openToken->size()) {
                            openToken->matched(closeToken, id);
                            closeToken->matched(openToken, id);

                            ++id;
                            break;
                        }
                        else if (openToken->size() == 3) {
                            // Split the open-token into a match for the close-token
                            // and a second for the leftovers.
                            openToken->disable();
                            TokenGroup g;
                            g.push_back(TokenPtr(new BoldOrItalicMarker(true,
                                openToken->tokenCharacter(), openToken->size()-
                                closeToken->size())));
                            g.push_back(TokenPtr(new BoldOrItalicMarker(true, openToken->tokenCharacter(), closeToken->size())));
                            TokenGroup::iterator after = ii;
                            ++after;
                            tgt.splice(after, g);
                            break;
                        }
                    }
                }
            }
        }

        // "Unmatch" invalidly-nested matches.
        stack<BoldOrItalicMarker*> openMatches;

        for (TokenGroup::iterator ii=tgt.begin(), iie=tgt.end(); ii!=iie; ++ii) {
            if ((*ii)->isMatchedOpenMarker()) {
                BoldOrItalicMarker *open=dynamic_cast<BoldOrItalicMarker*>(ii->get());
                openMatches.push(open);
            }
            else if ((*ii)->isMatchedCloseMarker()) {
                BoldOrItalicMarker *close=dynamic_cast<BoldOrItalicMarker*>(ii->get());

                if (close->id() != openMatches.top()->id()) {
                    close->matchedTo()->matched(0);
                    close->matched(0);
                }
                else {
                    openMatches.pop();

                    while (!openMatches.empty() && openMatches.top()->matchedTo()==0) {
                        openMatches.pop();
                    }
                }
            }
        }

        TokenGroup r;

        for (TokenGroup::iterator ii = tgt.begin(), iie = tgt.end(); ii != iie; ++ii) {
            if ((*ii)->text() && (*ii)->canContainMarkup()) {
                TokenGroup t = _encodeProcessedItems(*(*ii)->text(), replacements);

                r.splice(r.end(), t);
            }
            else {
                r.push_back(*ii);
            }
        }

        return r;
    }

    static TokenGroup RawText::_encodeProcessedItems(const string& src, ReplacementTable& replacements) {
        static const boost::regex cReplaced("\x01@(#?[0-9]*)@.+?\x01");

        TokenGroup r;

        string::const_iterator prev=src.begin();

        while (1) {
            boost::smatch m;

            if (boost::regex_search(prev, src.end(), m, cReplaced)) {
                string pre=string(prev, m[0].first);

                if (!pre.empty()) {
                    r.push_back(TokenPtr(new RawText(pre)));
                }

                prev       = m[0].second;
                string ref = m[1];

                if (ref[0]=='#') {
                    size_t n=boost::lexical_cast<size_t>(ref.substr(1));
                    r.push_back(TokenPtr(new EscapedCharacter(escapedCharacter(n))));
                }
                else if (!ref.empty()) {
                    size_t n=boost::lexical_cast<size_t>(ref);

                    assert(n<replacements.size());
                    r.push_back(replacements[n]);
                } // Otherwise just eat it
            }
            else {
                string pre = string(prev, src.end());

                if (!pre.empty()) {
                    r.push_back(TokenPtr(new RawText(pre)));
                }

                break;
            }
        }

        return r;
    }

    static string RawText::_restoreProcessedItems(const string &src, ReplacementTable& replacements) {
        static const boost::regex cReplaced("\x01@(#?[0-9]*)@.+?\x01");

        ostringstream r;
        string::const_iterator prev = src.begin();

        while (1) {
            boost::smatch m;

            if (boost::regex_search(prev, src.end(), m, cReplaced)) {
                string pre = string(prev, m[0].first);

                if (!pre.empty()) {
                    r << pre;
                }

                prev       = m[0].second;
                string ref = m[1];

                if (ref[0] == '#') {
                    size_t n = boost::lexical_cast<size_t>(ref.substr(1));
                    r << '\\' << escapedCharacter(n);
                }
                else if (!ref.empty()) {
                    size_t n=boost::lexical_cast<size_t>(ref);

                    assert(n < replacements.size());

                    replacements[n]->writeAsOriginal(r);
                } // Otherwise just eat it
            }
            else {
                string pre = string(prev, src.end());

                if (!pre.empty()) {
                    r << pre;
                }

                break;
            }
        }

        return r.str();
    }
}
