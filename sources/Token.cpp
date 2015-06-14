#include "../headers/Token.h"

using namespace std;


Token::Token() {

}

void Token::write_as_original(ostream& out) const {
    interprete_to_html(out);
}

void Token::write_token(size_t indent, ostream& out) const {
    out << string(indent * 2, ' ');
    write_token(out);
}

optional<TokenGroup> Token::process_span_elements(const LinkIds& idTable) {
    return none;
}

optional<const string&> Token::text() const {
    return none;
}

bool Token::canContainMarkup() const {
    return false;
}

bool Token::is_blank_line() const {
    return false;
}

bool Token::is_container() const {
    return false;
}

bool Token::is_unmatched_open_marker() const {
    return false;
}

bool Token::is_unmatched_close_marker() const {
    return false;
}

bool Token::isMatchedOpenMarker() const {
    return false;
}

bool Token::isMatchedCloseMarker() const {
    return false;
}

bool Token::inhibitParagraphs() const {
    return false;
}

void Token::pre_write(ostream& out) const {

}

void Token::post_write(ostream& out) const {

}
