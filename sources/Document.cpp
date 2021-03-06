#include <cassert>
#include <sstream>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include "../headers/Document.h"
// #include "../headers/Container.h"
#include "../headers/tags/BlankLine.h"
#include "../headers/tags/BlockQuote.h"
#include "../headers/tags/CodeBlock.h"
#include "../headers/tags/CodeSpan.h"
#include "../headers/tags/EscapedCharacter.h"
#include "../headers/tags/Header.h"
#include "../headers/tags/HtmlAnchorTag.h"
#include "../headers/tags/HtmlTag.h"
#include "../headers/tags/InlineHtmlBlock.h"
#include "../headers/tags/InlineHtmlComment.h"
#include "../headers/tags/InlineHtmlContents.h"
#include "../headers/tags/ListItem.h"
#include "../headers/tags/Paragraph.h"
#include "../headers/tags/RawText.h"
// // #include "tags/OrderedList.h"
// // #include "tags/UnorderedList.h"

using boost::optional;
using boost::none;

// TODO: bug si décommenté
// using TokenPtr;
// using CTokenGroupIter;

using namespace std;

const size_t Document::cSpacesPerInitialTab = 4;
const size_t Document::cDefaultSpacesPerTab = cSpacesPerInitialTab;

Document::Document(size_t spacesPerTab) : cSpacesPerTab(spacesPerTab),
                                          mTokenContainer(new Container),
                                          mIdTable(new LinkIds),
                                          mProcessed(false) {

}

Document::Document(istream& in, size_t spacesPerTab) : cSpacesPerTab(spacesPerTab),
                                                       mTokenContainer(new Container),
                                                       mIdTable(new LinkIds),
                                                       mProcessed(false) {
    read(in);
}

Document::~Document() {
    delete mIdTable;
}

struct HtmlTagInfo {
    string tagName;
    string extra;
    bool isClosingTag;
    size_t lengthOfToken; // In original string
};

const string cHtmlTokenSource("<((/?)([a-zA-Z0-9]+)(?:( +[a-zA-Z0-9]+?(?: ?= ?(\"|').*?\\5))*? */? *))>");
const boost::regex cHtmlTokenExpression(cHtmlTokenSource),
    cStartHtmlTokenExpression("^"+cHtmlTokenSource),
    cOneHtmlTokenExpression("^" + cHtmlTokenSource + "$");

enum ParseHtmlTagFlags { cAlone, cStarts };

optional<HtmlTagInfo> parseHtmlTag(string::const_iterator begin, string::const_iterator end, ParseHtmlTagFlags flags) {
    boost::smatch m;

    if (boost::regex_search(begin, end, m, (flags==cAlone ? cOneHtmlTokenExpression : cStartHtmlTokenExpression))) {
        HtmlTagInfo r;
        r.tagName = m[3];

        if (m[4].matched) {
            r.extra = m[4];
        }

        r.isClosingTag  = (m[2].length() > 0);
        r.lengthOfToken = m[0].length();

        return r;
    }

    return none;
}

TokenGroup parseInlineHtmlText(const string& src) {
    TokenGroup r;
    string::const_iterator prev = src.begin(), end = src.end();

    while (1) {
        boost::smatch m;

        if (boost::regex_search(prev, end, m, cHtmlTokenExpression)) {
            if (prev!=m[0].first) {
                r.push_back(TokenPtr(new InlineHtmlContents(string(prev, m[0].first))));
            }

            r.push_back(TokenPtr(new HtmlTag(m[1])));

            prev = m[0].second;
        }
        else {
            string eol;

            if (prev != end) {
                eol = string(prev, end);
            }

            eol += '\n';

            r.push_back(TokenPtr(new InlineHtmlContents(eol)));

            break;
        }
    }

    return r;
}

    // TODO: simplification
    // bool isHtmlCommentStart(string::const_iterator begin, string::const_iterator end) {
    //     // It can't be a single-line comment, those will already have been parsed
    //     // by is_blank_line.
    //     static const boost::regex cExpression("^<!--");
    //     return boost::regex_search(begin, end, cExpression);
    // }

    // bool isHtmlCommentEnd(string::const_iterator begin, string::const_iterator end) {
    //     static const boost::regex cExpression(".*-- *>$");
    //     return boost::regex_match(begin, end, cExpression);
    // }

/**
 * Détermine si la string passée en paramètre est vide ou commentée
 */
bool is_blank_line(const string& line) {
    static const boost::regex cExpression(" {0,3}(<--(.*)-- *> *)* *");
    return boost::regex_match(line, cExpression);
}

optional<TokenPtr> parseInlineHtml(CTokenGroupIter& i, CTokenGroupIter end) {
    // Preconditions: Previous line was blank, or this is the first line.
    if ((*i)->text()) {
        const string& line(*(*i)->text());

        bool tag     = false;
        bool comment = false;

        optional<HtmlTagInfo> tagInfo = parseHtmlTag(line.begin(), line.end(), cStarts);

        if (tagInfo && isValidTag(tagInfo->tagName)>1) {
            tag = true;
        }
        // TODO: simplification
        // else if (isHtmlCommentStart(line.begin(), line.end())) {
        //     comment = true;
        // }

        if (tag) {
            // Block continues until an HTML tag (alone) on a line followed by a blank line.
            TokenGroup contents;
            CTokenGroupIter firstLine = i;
            CTokenGroupIter prevLine  = i;
            size_t lines = 0;

            bool done = false;

            do {
                // We encode HTML tags so that their contents gets properly
                // handled -- i.e. "<div style=">"/>" becomes <div style="&gt;"/>
                if ((*i)->text()) {
                    TokenGroup t = parseInlineHtmlText(*(*i)->text());
                    contents.splice(contents.end(), t);
                }
                else {
                    contents.push_back(*i);
                }

                prevLine = i;
                ++i;
                ++lines;

                if (i!=end && (*i)->is_blank_line() && (*prevLine)->text()) {
                    if (prevLine==firstLine) {
                        done = true;
                    }
                    else {
                        const string& text(*(*prevLine)->text());
                        if (parseHtmlTag(text.begin(), text.end(), cAlone)) done = true;
                    }
                }
            } while (i!=end && !done);

            if (lines>1 || isValidTag(tagInfo->tagName, true)>1) {
                i = prevLine;

                return TokenPtr(new InlineHtmlBlock(contents));
            }
            else {
                // Single-line HTML "blocks" whose initial tags are span-tags
                // don't qualify as inline HTML.
                i = firstLine;

                return none;
            }
        }
        else if (comment) {
            // Comment continues until a closing tag is found; at present, it
            // also has to be the last thing on the line, and has to be
            // immediately followed by a blank line too.
            TokenGroup contents;
            CTokenGroupIter firstLine = i, prevLine = i;

            bool done = false;
            do {
                if ((*i)->text()) {
                    contents.push_back(TokenPtr(new InlineHtmlComment(*(*i)->text()+'\n')));
                }
                else {
                    contents.push_back(*i);
                }

                prevLine = i;
                ++i;

                if (i != end && (*i)->is_blank_line() && (*prevLine)->text()) {
                    if (prevLine==firstLine) {
                        done = true;
                    }
                    else {
                        const string& text(*(*prevLine)->text());
                        // if (isHtmlCommentEnd(text.begin(), text.end())) done = true;
                    }
                }
            } while (i!=end && !done);

            i = prevLine;

            return TokenPtr(new InlineHtmlBlock(contents));
        }
    }

    return none;
}

optional<string> isCodeBlockLine(CTokenGroupIter& i, CTokenGroupIter end) {
    if ((*i)->is_blank_line()) {
        // If we get here, we're already in a code block.
        ++i;

        if (i!=end) {
            optional<string> r = isCodeBlockLine(i, end);

            if (r) {
                return string("\n" + *r);
            }
        }

        --i;
    }
    else if ((*i)->text() && (*i)->canContainMarkup()) {
        const string& line(*(*i)->text());

        if (line.length() >= 4) {
            string::const_iterator si = line.begin(), sie = si+4;

            while (si!=sie && *si==' ') ++si;

            if (si==sie) {
                ++i;
                return string(si, line.end());
            }
        }
    }

    return none;
}

optional<TokenPtr> parseCodeBlock(CTokenGroupIter& i, CTokenGroupIter end) {
    if (!(*i)->is_blank_line()) {
        optional<string> contents = isCodeBlockLine(i, end);
        if (contents) {
            ostringstream out;
            out << *contents << '\n';
            while (i!=end) {
                contents = isCodeBlockLine(i, end);
                if (contents) out << *contents << '\n';
                else break;
            }
            return TokenPtr(new CodeBlock(out.str()));
        }
    }
    return none;
}

size_t countQuoteLevel(const string& prefixString) {
    size_t r = 0;
    for (string::const_iterator qi = prefixString.begin(),
        qie = prefixString.end(); qi!=qie; ++qi)
            if (*qi=='>') ++r;
    return r;
}

optional<TokenPtr> parseBlockQuote(CTokenGroupIter& i, CTokenGroupIter end) {
    static const boost::regex cBlockQuoteExpression("^((?: {0,3}>)+) (.*)$");
    // Useful captures: 1 = prefix, 2 = content

    if (!(*i)->is_blank_line() && (*i)->text() && (*i)->canContainMarkup()) {
        const string& line(*(*i)->text());
        boost::smatch m;
        if (boost::regex_match(line, m, cBlockQuoteExpression)) {
            size_t quoteLevel = countQuoteLevel(m[1]);
            boost::regex continuationExpression = boost::regex("^((?: {0,3}>){"+boost::lexical_cast<string>(quoteLevel)+"}) ?(.*)$");

            TokenGroup subTokens;
            subTokens.push_back(TokenPtr(new RawText(m[2])));

            // The next line can be a continuation of this quote (with or
            // without the prefix string) or a blank line. Blank lines are
            // treated as part of this quote if the following line is a
            // properly-prefixed quote line too, otherwise they terminate the
            // quote.
            ++i;
            while (i!=end) {
                if ((*i)->is_blank_line()) {
                    CTokenGroupIter ii = i;
                    ++ii;
                    if (ii==end) {
                        i = ii;
                        break;
                    } else {
                        const string& line(*(*ii)->text());
                        if (boost::regex_match(line, m, continuationExpression)) {
                            if (m[1].matched && m[1].length()>0) {
                                i=++ii;
                                subTokens.push_back(TokenPtr(new BlankLine));
                                subTokens.push_back(TokenPtr(new RawText(m[2])));
                            } else break;
                        } else break;
                    }
                } else {
                    const string& line(*(*i)->text());
                    if (boost::regex_match(line, m, continuationExpression)) {
                        assert(m[2].matched);
                        if (!is_blank_line(m[2])) subTokens.push_back(TokenPtr(new RawText(m[2])));
                        else subTokens.push_back(TokenPtr(new BlankLine(m[2])));
                        ++i;
                    } else break;
                }
            }

            return TokenPtr(new BlockQuote(subTokens));
        }
    }

    return none;
}

optional<TokenPtr> parseListBlock(CTokenGroupIter& i, CTokenGroupIter end, bool sub = false) {
    // static const boost::regex cUnorderedListExpression("^( *)([*+-]) +([^*-].*)$");
    // static const boost::regex cOrderedListExpression("^( *)([0-9]+)\\. +(.*)$");

    enum ListType { cNone, cUnordered, cOrdered };
    ListType type = cNone;
    if (!(*i)->is_blank_line() && (*i)->text() && (*i)->canContainMarkup()) {
        boost::regex nextItemExpression, startSublistExpression;
        size_t indent = 0;

        const string& line(*(*i)->text());

        //cerr << "IsList? " << line << endl;

        TokenGroup subTokens, subItemTokens;

        boost::smatch m;

       /* if (boost::regex_match(line, m, cUnorderedListExpression)) {
            indent = m[1].length();
            if (sub || indent<4) {
                type = cUnordered;
                char startChar=*m[2].first;
                subItemTokens.push_back(TokenPtr(new RawText(m[3])));

                ostringstream next;
                next << "^" << string(indent, ' ') << "\\" << startChar << " +([^*-].*)$";
                nextItemExpression = next.str();
            }
        } else if (boost::regex_match(line, m, cOrderedListExpression)) {
            indent = m[1].length();
            if (sub || indent<4) {
                type = cOrdered;
                subItemTokens.push_back(TokenPtr(new RawText(m[3])));

                ostringstream next;
                next << "^" << string(indent, ' ') << "[0-9]+\\. +(.*)$";
                nextItemExpression = next.str();
            }
        }*/

        if (type != cNone) {
            CTokenGroupIter originalI = i;
            size_t itemCount = 1;
            ostringstream sub;

            sub << "^" << string(indent, ' ') << " +(([*+-])|([0-9]+\\.)) +.*$";

            startSublistExpression = sub.str();

            // There are several options for the next line. It's another item in
            // this list (in which case this one is done); it's a continuation
            // of this line (collect it and keep going); it's the first item in
            // a sub-list (call this function recursively to collect it), it's
            // the next item in the parent list (this one is ended); or it's
            // blank.
            //
            // A blank line requires looking ahead. If the next line is an item
            // for this list, then switch this list into paragraph-items mode
            // and continue processing. If it's indented by four or more spaces
            // (more than the list itself), then it's another continuation of
            // the current item. Otherwise it's either a new paragraph (and this
            // list is ended) or the beginning of a sub-list.
            static const boost::regex cContinuedItemExpression("^ *([^ ].*)$");

            boost::regex continuedAfterBlankLineExpression("^ {"+
                boost::lexical_cast<string>(indent+4)+"}([^ ].*)$");
            boost::regex codeBlockAfterBlankLineExpression("^ {"+
                boost::lexical_cast<string>(indent+8)+"}(.*)$");

            enum NextItemType { cUnknown, cEndOfList, cAnotherItem };

            NextItemType nextItem = cUnknown;
            bool setParagraphMode = false;

            ++i;

            while (i!=end) {
                if ((*i)->is_blank_line()) {
                    CTokenGroupIter ii = i;
                    ++ii;
                    if (ii==end) {
                        i = ii;
                        nextItem = cEndOfList;
                    }
                    else if ((*ii)->text()) {
                        const string& line(*(*ii)->text());

                        if (boost::regex_match(line, startSublistExpression)) {
                            setParagraphMode = true;
                            ++itemCount;
                            i = ii;
                            optional<TokenPtr> p = parseListBlock(i, end, true);
                            assert(p);
                            subItemTokens.push_back(*p);
                            continue;
                        }
                        else if (boost::regex_match(line, m, nextItemExpression)) {
                            setParagraphMode = true;
                            i = ii;
                            nextItem = cAnotherItem;
                        }
                        else if (boost::regex_match(line, m, continuedAfterBlankLineExpression)) {
                            assert(m[1].matched);
                            subItemTokens.push_back(TokenPtr(new BlankLine()));
                            subItemTokens.push_back(TokenPtr(new RawText(m[1])));
                            i=++ii;
                            continue;
                        }
                        else if (boost::regex_match(line, m, codeBlockAfterBlankLineExpression)) {
                            setParagraphMode = true;
                            ++itemCount;
                            assert(m[1].matched);
                            subItemTokens.push_back(TokenPtr(new BlankLine()));

                            string codeBlock = m[1]+'\n';
                            ++ii;

                            while (ii!=end) {
                                if ((*ii)->is_blank_line()) {
                                    CTokenGroupIter iii = ii;
                                    ++iii;
                                    const string& nextLine(*(*iii)->text());

                                    if (boost::regex_match(nextLine, m, codeBlockAfterBlankLineExpression)) {
                                        codeBlock+='\n'+m[1]+'\n';
                                        ii = iii;
                                    }
                                    else break;
                                }
                                else if ((*ii)->text()) {
                                    const string& line(*(*ii)->text());

                                    if (boost::regex_match(line, m, codeBlockAfterBlankLineExpression)) {
                                        codeBlock+=m[1]+'\n';
                                    }
                                    else break;
                                }
                                else break;

                                ++ii;
                            }

                            subItemTokens.push_back(TokenPtr(new CodeBlock(codeBlock)));
                            i = ii;

                            continue;
                        }
                        else {
                            nextItem = cEndOfList;
                        }
                    }
                    else break;
                }
                else if ((*i)->text()) {
                    const string& line(*(*i)->text());

                    if (boost::regex_match(line, startSublistExpression)) {
                        ++itemCount;
                        optional<TokenPtr> p = parseListBlock(i, end, true);
                        assert(p);
                        subItemTokens.push_back(*p);
                        continue;
                    }
                    else if (boost::regex_match(line, m, nextItemExpression)) {
                        nextItem = cAnotherItem;
                    }
                    else {
                        /*if (boost::regex_match(line, m, cUnorderedListExpression)
                            || boost::regex_match(line, m, cOrderedListExpression))
                        {
                            // Belongs to the parent list
                            nextItem = cEndOfList;
                        } else {*/
                            boost::regex_match(line, m, cContinuedItemExpression);
                            assert(m[1].matched);
                            subItemTokens.push_back(TokenPtr(new RawText(m[1])));
                            ++i;
                            continue;
                        //}
                    }
                }
                else nextItem = cEndOfList;

                if (!subItemTokens.empty()) {
                    subTokens.push_back(TokenPtr(new ListItem(subItemTokens)));
                    subItemTokens.clear();
                }

                assert(nextItem!=cUnknown);

                if (nextItem==cAnotherItem) {
                    subItemTokens.push_back(TokenPtr(new RawText(m[1])));
                    ++itemCount;
                    ++i;
                }
                else { // nextItem==cEndOfList
                    break;
                }
            }

            // In case we hit the end with an unterminated item...
            if (!subItemTokens.empty()) {
                subTokens.push_back(TokenPtr(new ListItem(subItemTokens)));
                subItemTokens.clear();
            }

            if (itemCount>1 || indent!=0) {
                // if (type==cUnordered) {
                //     return TokenPtr(new UnorderedList(subTokens, setParagraphMode));
                // } else {
                //     return TokenPtr(new OrderedList(subTokens, setParagraphMode));
                // }
            }
            else {
                // It looked like a list, but turned out to be a false alarm.
                i = originalI;
                return none;
            }
        }
    }

    return none;
}

bool parseReference(CTokenGroupIter& i, CTokenGroupIter end, LinkIds &idTable) {
    if ((*i)->text()) {
        static const boost::regex cReference("^ {0,3}\\[(.+)\\]: +<?([^ >]+)>?(?: *(?:('|\")(.*)\\3)|(?:\\((.*)\\)))?$");
        // Useful captures: 1 = id, 2 = url, 4/5 = title

        const string& line1(*(*i)->text());
        boost::smatch m;

        if (boost::regex_match(line1, m, cReference)) {
            string id(m[1]), url(m[2]), title;
            if (m[4].matched) title = m[4];
            else if (m[5].matched) title = m[5];
            else {
                CTokenGroupIter ii = i;
                ++ii;

                if (ii!=end && (*ii)->text()) {
                    // It could be on the next line
                    static const boost::regex cSeparateTitle("^ *(?:(?:('|\")(.*)\\1)|(?:\\((.*)\\))) *$");
                    // Useful Captures: 2/3 = title

                    const string& line2(*(*ii)->text());

                    if (boost::regex_match(line2, m, cSeparateTitle)) {
                        ++i;
                        title=(m[2].matched ? m[2] : m[3]);
                    }
                }
            }

            idTable.add(id, url, title);

            return true;
        }
    }

    return false;
}

void flushParagraph(string& paragraphText, TokenGroup& paragraphTokens, TokenGroup& finalTokens, bool noParagraphs) {
    if (!paragraphText.empty()) {
        paragraphTokens.push_back(TokenPtr(new RawText(paragraphText)));
        paragraphText.clear();
    }

    if (!paragraphTokens.empty()) {
        if (noParagraphs) {
            if (paragraphTokens.size()>1) {
                finalTokens.push_back(TokenPtr(new Container(paragraphTokens)));
            }
            else finalTokens.push_back(*paragraphTokens.begin());
        }
        else finalTokens.push_back(TokenPtr(new Paragraph(paragraphTokens)));

        paragraphTokens.clear();
    }
}

optional<TokenPtr> parseHeader(CTokenGroupIter& i, CTokenGroupIter end) {
    if (!(*i)->is_blank_line() && (*i)->text() && (*i)->canContainMarkup()) {
        // Hash-mark type
        static const boost::regex cHashHeaders("^(#{1,6}) +(.*?) *#*$");
        const string& line=*(*i)->text();
        boost::smatch m;

        if (boost::regex_match(line, m, cHashHeaders))
            return TokenPtr(new Header(m[1].length(), m[2]));

        // Underlined type
        CTokenGroupIter ii = i;
        ++ii;
        if (ii!=end && !(*ii)->is_blank_line() && (*ii)->text() && (*ii)->canContainMarkup()) {
            static const boost::regex cUnderlinedHeaders("^([-=])\\1*$");
            const string& line=*(*ii)->text();

            if (boost::regex_match(line, m, cUnderlinedHeaders)) {
                char typeChar = string(m[1])[0];
                TokenPtr p = TokenPtr(new Header((typeChar=='='
                    ? 1 : 2), *(*i)->text()));
                i = ii;
                return p;
            }
        }
    }

    return none;
}

optional<TokenPtr> parseHorizontalRule(CTokenGroupIter& i, CTokenGroupIter end) {
    if (!(*i)->is_blank_line() && (*i)->text() && (*i)->canContainMarkup()) {
        static const boost::regex cHorizontalRules("^ {0,3}((?:-|\\*|_) *){3,}$");
        const string& line=*(*i)->text();

        if (boost::regex_match(line, cHorizontalRules)) {
            return TokenPtr(new HtmlTag("hr/"));
        }
    }

    return none;
}

bool Document::read(const string& src) {
    istringstream in(src);

    return read(in);
}

bool Document::getline(istream& in, string& line) {
    // Handles \n, \r, and \r\n (and even \n\r) on any system. Also does tab-
    // expansion, since this is the most efficient place for it.
    line.clear();

    bool initialWhitespace = true;
    char c;

    while (in.get(c)) {
        if (c == '\r') {
            if ((in.get(c)) && c != '\n') {
                in.unget();
            }

            return true;
        }
        else if (c == '\n') {
            if ((in.get(c)) && c != '\r') {
                in.unget();
            }

            return true;
        }
        else if (c == '\t') {
            size_t convert = (initialWhitespace ? cSpacesPerInitialTab : cSpacesPerTab);
            line += string(convert - (line.length() % convert), ' ');
        }
        else {
            line.push_back(c);

            if (c != ' ') {
                initialWhitespace = false;
            }
        }
    }

    return !line.empty();
}

bool Document::read(istream& in) {
    if (mProcessed) return false;

    Container *tokens = dynamic_cast<Container*>(mTokenContainer.get());
    assert(tokens!=0);

    string line;
    TokenGroup tgt;

    while (getline(in, line)) {
        if (is_blank_line(line)) {
            tgt.push_back(TokenPtr(new BlankLine(line)));
        } else {
            tgt.push_back(TokenPtr(new RawText(line)));
        }
    }

    tokens->appendSubtokens(tgt);

    return true;
}

/**
 * Lancement de l'interprétation
 */
void Document::write(ostream& out) {
    process();
    mTokenContainer->interprete_to_html(out);
}

/**
 * Pour débug, écrit les tokens plutôt que les balises HTML.
 */
void Document::write_tokens(ostream& out) {
    process();
    mTokenContainer->write_token(0, out);
}

/**
 * Méthode de réunification de sous-méthodes
 */
void Document::process() {
    if (!mProcessed) {
        merge_multiline_html_tags();
        process_inline_html_and_references();
        process_blocks_items(mTokenContainer);
        process_paragraph_lines(mTokenContainer);

        mTokenContainer->process_span_elements(*mIdTable);

        mProcessed = true;
    }
}

/**
 * S'applique forcément sur les balises HTML de type Container (imbrication impossible sinon)
 */
void Document::merge_multiline_html_tags() {
    // Ensemble des caractères de début de balises HTML
    static const boost::regex html_start("<((/?)([a-zA-Z0-9]+)(?:( +[a-zA-Z0-9]+?(?: ?= ?(\"|').*?\\5))*? */? *))$");
    // Ensemble des caractères de fin de balises HTML
    static const boost::regex html_end("^ *((?:( +[a-zA-Z0-9]+?(?: ?= ?(\"|').*?\\3))*? */? *))>");

    TokenGroup processed;

    // Cast le token courant (classe ToekPtr) en classe Container
    // mTokenContainer.get() retourne un pointeur de TokenPtr
    Container *tokens = dynamic_cast<Container *>(mTokenContainer.get());

    // simplification
    // assert(tokens != 0);

    // Pour tous les sous tokens du token courant
    for (TokenGroup::const_iterator i = tokens->subTokens().begin(), ie = tokens->subTokens().end(); i != ie; i++) {
        // Si le sous token courant a un texte
        // et qu'il correspond à la regex de caractère de début de balise HTML
        if ((*i)->text() && boost::regex_match(*(*i)->text(), html_start)) {
            // Retient le sous token courant
            TokenGroup::const_iterator i2 = i;

            i2++;

            // Si le sous token courant n'est pas le dernier du token courant
            // et qu'il a un texte
            // et qu'il correspond à la regex de caractère de fin de balise HTML
            if (i2 != tokens->subTokens().end() && (*i2)->text() && boost::regex_match(*(*i2)->text(), html_end)) {
                // Merge les textes du sous token courant et token courant
                processed.push_back(TokenPtr(new RawText(*(*i)->text() + ' ' + *(*i2)->text())));
                i++;
                continue;
            }
        }

        processed.push_back(*i);
    }

    tokens->swap_subtokens(processed);
}

void Document::process_inline_html_and_references() {
    TokenGroup processed;

    Container *tokens = dynamic_cast<Container *>(mTokenContainer.get());

    assert(tokens!=0);

    for (TokenGroup::const_iterator ii = tokens->subTokens().begin(), iie = tokens->subTokens().end(); ii!=iie; ++ii) {
        if ((*ii)->text()) {
            if (processed.empty() || processed.back()->is_blank_line()) {
                optional<TokenPtr> inlineHtml = parseInlineHtml(ii, iie);

                if (inlineHtml) {
                    processed.push_back(*inlineHtml);

                    if (ii == iie) {
                        break;
                    }

                    continue;
                }
            }

            if (parseReference(ii, iie, *mIdTable)) {
                if (ii == iie) {
                    break;
                }

                continue;
            }

            // If it gets down here, just store it in its current (raw text)
            // form. We'll group the raw text lines into paragraphs in a
            // later pass, since we can't easily tell where paragraphs
            // end until then.
        }

        processed.push_back(*ii);
    }

    tokens->swap_subtokens(processed);
}

/**
 *
 */
void Document::process_blocks_items(TokenPtr inTokenContainer) {
    if (!inTokenContainer->is_container()) {
        return;
    }

    Container *tokens = dynamic_cast<Container *>(inTokenContainer.get());

    assert(tokens != 0);

    TokenGroup processed;

    for (TokenGroup::const_iterator ii = tokens->subTokens().begin(), iie = tokens->subTokens().end(); ii != iie; ++ii) {
        if ((*ii)->text()) {
            optional<TokenPtr> subitem;

            if (!subitem) subitem = parseHeader(ii, iie);
            if (!subitem) subitem = parseHorizontalRule(ii, iie);
            if (!subitem) subitem = parseListBlock(ii, iie);
            if (!subitem) subitem = parseBlockQuote(ii, iie);
            if (!subitem) subitem = parseCodeBlock(ii, iie);

            if (subitem) {
                process_blocks_items(*subitem);
                processed.push_back(*subitem);

                if (ii == iie) {
                    break;
                }

                continue;
            }
            else {
                processed.push_back(*ii);
            }
        }
        else if ((*ii)->is_container()) {
            process_blocks_items(*ii);
            processed.push_back(*ii);
        }
    }

    tokens->swap_subtokens(processed);
}

void Document::process_paragraph_lines(TokenPtr inTokenContainer) {
    Container *tokens = dynamic_cast<Container *>(inTokenContainer.get());

    assert(tokens != 0);

    bool noPara = tokens->inhibitParagraphs();

    for (TokenGroup::const_iterator ii = tokens->subTokens().begin(), iie = tokens->subTokens().end(); ii != iie; ++ii) {
        if ((*ii)->is_container()) {
            process_paragraph_lines(*ii);
        }
    }

    TokenGroup processed;
    string paragraphText;
    TokenGroup paragraphTokens;

    for (TokenGroup::const_iterator ii = tokens->subTokens().begin(), iie = tokens->subTokens().end(); ii != iie; ++ii) {
        if ((*ii)->text() && (*ii)->canContainMarkup() && !(*ii)->inhibitParagraphs()) {
            static const boost::regex cExpression("^(.*)  $");

            if (!paragraphText.empty()) {
                paragraphText += " ";
            }

            boost::smatch m;

            if (boost::regex_match(*(*ii)->text(), m, cExpression)) {
                paragraphText += m[1];

                flushParagraph(paragraphText, paragraphTokens, processed, noPara);
                processed.push_back(TokenPtr(new HtmlTag("br/")));
            }
            else {
                paragraphText += *(*ii)->text();
            }
        }
        else {
            flushParagraph(paragraphText, paragraphTokens, processed, noPara);
            processed.push_back(*ii);
        }
    }

    // Make sure the last paragraph is properly flushed too.
    flushParagraph(paragraphText, paragraphTokens, processed, noPara);

    tokens->swap_subtokens(processed);
}
