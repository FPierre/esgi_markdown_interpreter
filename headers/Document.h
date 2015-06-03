#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>
#include <iostream>
#include <string>
#include <list>

using namespace std;

using boost::optional;
using boost::none;

// Forward references
class Token;
class LinkIds;

// shared_ptr : http://openclassrooms.com/courses/c-gerer-correctement-ses-allocations-dynamiques
// et : http://openclassrooms.com/courses/c-gerer-correctement-ses-allocations-dynamiques/deuxieme-exemple-boost-shared-ptr
typedef boost::shared_ptr<Token> TokenPtr;
typedef list<TokenPtr> TokenGroup;

class Document {
    public:
    Document(size_t spacesPerTab = cDefaultSpacesPerTab);
    Document(istream& in, size_t spacesPerTab = cDefaultSpacesPerTab);
    ~Document();

    bool read(const string&);
    bool read(istream&);
    void write(ostream&);
    void write_tokens(ostream&);

    private:
    static const size_t cSpacesPerInitialTab, cDefaultSpacesPerTab;
    const size_t cSpacesPerTab;
    TokenPtr mTokenContainer;
    LinkIds *mIdTable;
    bool mProcessed;

    bool getline(istream& in, string& line);
    void process();
    void mergeMultilineHtmlTags();
    void processInlineHtmlAndReferences();
    void processBlocksItems(TokenPtr inTokenContainer);
    void processParagraphLines(TokenPtr inTokenContainer);
};

#endif
