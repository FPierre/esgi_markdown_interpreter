#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <iostream>
#include <string>
#include <list>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>

using namespace std;

namespace markdown {
	using boost::optional;
	using boost::none;

	// Forward references.
	class Token;
	class LinkIds;

	typedef boost::shared_ptr<Token> TokenPtr;
	typedef list<TokenPtr> TokenGroup;

	class Document : private boost::noncopyable {
		public:
		Document(size_t spacesPerTab = cDefaultSpacesPerTab);
		Document(istream& in, size_t spacesPerTab = cDefaultSpacesPerTab);
		~Document();

		// You can call read() functions multiple times before writing if
		// desirable. Once the document has been processed for writing, it can't
		// accept any more input.
		bool read(const string&);
		bool read(istream&);
		void write(ostream&);       // Mode normal
		void writeTokens(ostream&); // Pour débugger

		// The class is marked noncopyable because it uses reference-counted
		// links to things that get changed during processing. If you want to
		// copy it, use the `copy` function to explicitly say that.
		Document copy() const; // TODO: Copy function not yet written.

		private:
		static const size_t cSpacesPerInitialTab, cDefaultSpacesPerTab;
		const size_t cSpacesPerTab;
		TokenPtr mTokenContainer;
		LinkIds *mIdTable;
		bool mProcessed;
		bool _getline(istream& in, string& line);
		void process();
		void _mergeMultilineHtmlTags();
		void _processInlineHtmlAndReferences();
		void _processBlocksItems(TokenPtr inTokenContainer);
		void _processParagraphLines(TokenPtr inTokenContainer);
	};
}

#endif
