// #include "OrderedList.h"

// namespace markdown {
//     OrderedList::OrderedList(const TokenGroup& contents, bool paragraphMode) : UnorderedList(contents, paragraphMode) {

//     }

//     TokenPtr OrderedList::clone(const TokenGroup& newContents) const {
//         return TokenPtr(new OrderedList(newContents));
//     }

//     string OrderedList::container_name() const {
//         return "OrderedList";
//     }

//     void OrderedList::pre_write(ostream& out) const {
//         out << "<ol>\n";
//     }

//     void OrderedList::post_write(ostream& out) const {
//         out << "</ol>\n\n";
//     }
// }
