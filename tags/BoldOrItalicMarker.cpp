#include "BoldOrItalicMarker.h"

namespace markdown {
    BoldOrItalicMarker::BoldOrItalicMarker(bool open, char c, size_t size) : mOpenMarker(open),
                                                                             mTokenCharacter(c),
                                                                             mSize(size),
                                                                             mMatch(0),
                                                                             mCannotMatch(false),
                                                                             mDisabled(false),
                                                                             mId(-1) {

    }

    bool BoldOrItalicMarker::isUnmatchedOpenMarker() const {
        return (mOpenMarker && mMatch==0 && !mCannotMatch);
    }

    bool BoldOrItalicMarker::isUnmatchedCloseMarker() const {
        return (!mOpenMarker && mMatch==0 && !mCannotMatch);
    }

    bool BoldOrItalicMarker::isMatchedOpenMarker() const {
        return (mOpenMarker && mMatch!=0);
    }

    bool BoldOrItalicMarker::isMatchedCloseMarker() const {
        return (!mOpenMarker && mMatch!=0);
    }

    bool BoldOrItalicMarker::isOpenMarker() const {
        return mOpenMarker;
    }

    char BoldOrItalicMarker::tokenCharacter() const {
        return mTokenCharacter;
    }

    size_t BoldOrItalicMarker::size() const {
        return mSize;
    }

    bool BoldOrItalicMarker::matched() const {
        return (mMatch!=0);
    }

    BoldOrItalicMarker* BoldOrItalicMarker::matchedTo() const {
        return mMatch;
    }

    int BoldOrItalicMarker::id() const {
        return mId;
    }

    void BoldOrItalicMarker::matched(BoldOrItalicMarker *match, int id=-1) {
        mMatch=match; mId=id;
    }

    void BoldOrItalicMarker::cannotMatch(bool set) {
        mCannotMatch=set;
    }

    void BoldOrItalicMarker::disable() {
        mCannotMatch=mDisabled=true;
    }

    void BoldOrItalicMarker::interprete_to_html(ostream& out) const {
        if (!mDisabled) {
            if (mMatch != 0) {
                assert(mSize >= 1 && mSize <= 3);

                if (mOpenMarker) {
                    out << (mSize == 1 ? "<em>" : mSize == 2 ? "<strong>" : "<strong><em>");

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
}
