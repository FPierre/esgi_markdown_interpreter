# g++ main.cpp Document.cpp tokens.cpp -lboost_regex
# http://linux.leunen.com/?p=21

# http://www.haverford.edu/cmsc/course-pages/usingMake.html

# A	rajouter dans CFLAFS : -Wall -Wextra -pedantic

CC = g++
CFLAGS = -lboost_regex -std=c++11
EXEC_NAME = interpreter
OBJ_FILES = BlankLine.o BlockQuote.o CodeBlock.o CodeSpan.o EscapedCharacter.o Header.o HtmlAnchorTag.o HtmlTag.o InlineHtmlBlock.o InlineHtmlComment.o InlineHtmlContents.o ListItem.o Paragraph.o RawText.o Container.o Document.o LinkIds.o Options.o TextHolder.o Token.o main.o

all: $(EXEC_NAME)

clean:
	rm $(EXEC_NAME)

$(EXEC_NAME): $(OBJ_FILES)
	$(CC) -o $(EXEC_NAME) $(OBJ_FILES) $(CFLAGS)

%.o: sources/tags/%.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: sources/%.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
