// #include "OrderedList.h"

// namespace markdown {
//     OrderedList::OrderedList(const TokenGroup& contents, bool paragraphMode) : UnorderedList(contents, paragraphMode) {

//     }

//     TokenPtr OrderedList::clone(const TokenGroup& newContents) const {
//         return TokenPtr(new OrderedList(newContents));
//     }

//     string OrderedList::containerName() const {
//         return "OrderedList";
//     }

//     void OrderedList::preWrite(ostream& out) const {
//         out << "<ol>\n";
//     }

//     void OrderedList::postWrite(ostream& out) const {
//         out << "</ol>\n\n";
//     }
// }
