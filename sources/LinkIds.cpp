#include "../headers/LinkIds.h"


    optional<LinkIds::Target> LinkIds::find(const string& id) const {
        Table::const_iterator i=mTable.find(_scrubKey(id));
        if (i!=mTable.end()) return i->second;
        else return none;
    }

    void LinkIds::add(const string& id, const string& url, const string& title) {
        mTable.insert(make_pair(_scrubKey(id), Target(url, title)));
    }

    string LinkIds::_scrubKey(string str) {
        boost::algorithm::to_lower(str);

        return str;
    }

