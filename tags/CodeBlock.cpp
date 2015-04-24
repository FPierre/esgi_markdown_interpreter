#include "CodeBlock.h"

namespace markdown {
    CodeBlock::CodeBlock(const string& actualContents) : TextHolder(actualContents, false, cDoubleAmps|cAngles|cQuotes) {

    }

    void CodeBlock::interprete_to_html(ostream& out) const {
        out << "<pre><code>";
        TextHolder::interprete_to_html(out);
        out << "</code></pre>\n\n";
    }

    void CodeBlock::writeToken(ostream& out) const {
        out << "CodeBlock: " << *text() << '\n';
    }
}
