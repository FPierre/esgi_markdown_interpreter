#include <ostream>
#include <string>

#include <vector>
#include "Document.h"

using namespace std;

namespace markdown {
    typedef TokenGroup::iterator TokenGroupIter;
    typedef TokenGroup::const_iterator CTokenGroupIter;

    class LinkIds {
        public:
        struct Target {
            string url;
            string title;
            Target(const string& url_, const string& title_) : url(url_), title(title_) { }
        };
        optional<Target> find(const string& id) const;
        void add(const string& id, const string& url, const string& title);

        private:
        typedef boost::unordered_map<string, Target> Table;
        static string _scrubKey(string str);
        Table mTable;
    };

    class Token {
        public:
        Token();
        virtual void interprete_to_html(ostream&) const = 0;
        virtual void writeAsOriginal(ostream& out) const;
        virtual void writeToken(ostream& out) const = 0;
        virtual void writeToken(size_t indent, ostream& out) const;
        virtual optional<TokenGroup> processSpanElements(const LinkIds& idTable);
        virtual optional<const string&> text() const;
        virtual bool canContainMarkup() const;
        virtual bool isBlankLine() const;
        virtual bool isContainer() const;
        virtual bool isUnmatchedOpenMarker() const;
        virtual bool isUnmatchedCloseMarker() const;
        virtual bool isMatchedOpenMarker() const;
        virtual bool isMatchedCloseMarker() const;
        virtual bool inhibitParagraphs() const;

        protected:
        virtual void preWrite(ostream& out) const;
        virtual void postWrite(ostream& out) const;
    };
}
