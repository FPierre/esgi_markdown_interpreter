#include "../headers/Container.h"

Container::Container(const TokenGroup& contents) : mSubTokens(contents),
                                                   mParagraphMode(false) {

}

const TokenGroup& Container::subTokens() const {
    return mSubTokens;
}

void Container::appendSubtokens(TokenGroup& tokens) {
    mSubTokens.splice(mSubTokens.end(), tokens);
}

void Container::swapSubtokens(TokenGroup& tokens) {
    mSubTokens.swap(tokens);
}

bool Container::isContainer() const {
    return true;
}

void Container::interprete_to_html(ostream& out) const {
    pre_write(out);

    for (CTokenGroupIter i = mSubTokens.begin(), ie = mSubTokens.end(); i != ie; ++i) {
        (*i)->interprete_to_html(out);
    }

    post_write(out);
}

void Container::write_token(ostream& out) const {
    out << "Container: error!" << '\n';
}

void Container::write_token(size_t indent, ostream& out) const {
    out << string(indent * 2, ' ') << container_name() << endl;

    for (CTokenGroupIter ii=mSubTokens.begin(), iie=mSubTokens.end(); ii!=iie; ++ii) {
        (*ii)->write_token(indent + 1, out);
    }
}

optional<TokenGroup> Container::processSpanElements(const LinkIds& idTable) {
    TokenGroup t;

    for (CTokenGroupIter ii = mSubTokens.begin(), iie = mSubTokens.end(); ii != iie; ++ii) {
        if ((*ii)->text()) {
            optional<TokenGroup> subt = (*ii)->processSpanElements(idTable);

            if (subt) {
                if (subt->size()>1) {
                    t.push_back(TokenPtr(new Container(*subt)));
                }
                else if (!subt->empty()) {
                    t.push_back(*subt->begin());
                }
            }
            else {
                t.push_back(*ii);
            }
        }
        else {
            optional<TokenGroup> subt = (*ii)->processSpanElements(idTable);

            if (subt) {
                const Container *c = dynamic_cast<const Container *>((*ii).get());

                assert(c != 0);

                t.push_back(c->clone(*subt));
            }
            else {
                t.push_back(*ii);
            }
        }
    }

    swapSubtokens(t);

    return none;
}

TokenPtr Container::clone(const TokenGroup& newContents) const {
    return TokenPtr(new Container(newContents));
}

string Container::container_name() const {
    return "Container";
}
