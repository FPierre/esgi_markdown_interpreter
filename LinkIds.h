#ifndef LINKIDS_H
#define LINKIDS_H

#include <string>

using boost::optional;
using boost::unordered_map;

using namespace std;

namespace markdown {
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
        typedef unordered_map<string, Target> Table;
        static string _scrubKey(string str);
        Table mTable;
    };
}

#endif
