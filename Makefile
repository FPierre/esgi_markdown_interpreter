# g++ main.cpp Document.cpp tokens.cpp -lboost_regex
# http://linux.leunen.com/?p=21

CC = g++
CFLAGS = -lboost_regex --std=c++11
EXEC_NAME = interpreter
OBJ_FILES = tags/BlankLine.o tags/BlockQuote.o tags/CodeBlock.o tags/CodeSpan.o tags/EscapedCharacter.o tags/Header.o tags/HtmlAnchorTag.o tags/HtmlTag.o tags/InlineHtmlBlock.o tags/InlineHtmlComment.o tags/InlineHtmlContents.o tags/ListItem.o tags/OrderedList.o tags/Paragraph.o tags/RawText.o tags/UnorderedList.o Container.o Document.o LinkIds.o Options.o TextHolder.o Token.o main.o
INSTALL_DIR = .

all: $(EXEC_NAME)

clean:
	rm $(EXEC_NAME)

$(EXEC_NAME): $(OBJ_FILES)
	$(CC) -o $(EXEC_NAME) $(OBJ_FILES)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

install:
	cp $(EXEC_NAME) $(INSTALL_DIR)

clean2:
	rm $(OBJ_FILES)
