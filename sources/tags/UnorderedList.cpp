// #include "UnorderedList.h"
// #include "ListItem.h"

// namespace markdown {
//     UnorderedList::UnorderedList(const TokenGroup& contents, bool paragraphMode) {
//         if (paragraphMode) {
//             // Change each of the text items into paragraphs
//             for (CTokenGroupIter i = contents.begin(), ie = contents.end(); i != ie; ++i) {
//                 ListItem *item = dynamic_cast<ListItem *>((*i).get());

//                 assert(item != 0);

//                 item->inhibitParagraphs(false);
//                 mSubTokens.push_back(*i);
//             }
//         }
//         else {
//             mSubTokens=contents;
//         }
//     }

//     void UnorderedList::pre_write(ostream& out) const {
//         out << "\n<ul>\n";
//     }

//     void UnorderedList::post_write(ostream& out) const {
//         out << "</ul>\n\n";
//     }

//     TokenPtr UnorderedList::clone(const TokenGroup& newContents) const {
//         return TokenPtr(new UnorderedList(newContents));
//     }

//     string UnorderedList::containerName() const {
//         return "UnorderedList";
//     }
// }
