// #include "Image.h"

// namespace markdown {
//     Image::Image(const string& altText, const string& url, const string& title) : mAltText(altText),
//                                                                                   mUrl(url),
//                                                                                   mTitle(title) {

//     }

//     void Image::interprete_to_html(ostream& out) const {
//         out << "<img src=\"" << mUrl << "\" alt=\"" << mAltText << "\"";

//         if (!mTitle.empty()) {
//             out << " title=\"" << mTitle << "\"";
//         }

//         out << "/>";
//     }

//     Image::void write_token(ostream& out) const {
//         out << "Image: " << mUrl << '\n';
//     }
// }
