SRC_DIR = src
FLEX_FILE := $(SRC_DIR)/lexer.lex
FLEX_SRC := $(FLEX_FILE:.lex=.c)
BISON_FILE := $(SRC_DIR)/parser.y
BISON_SRC := $(BISON_FILE:.y=.tab.c)
BISON_HEADERS := $(BISON_SRC:.c=.h)

all: flex

flex: bison $(FLEX_SRC)

bison: $(BISON_SRC)

$(BISON_SRC) : %.tab.c : %.y
	bison -o $@ -d $<

$(FLEX_SRC) : %.c : %.lex
	flex -o $@ $<

clean: CLEAN_FLEX CLEAN_BISON

CLEAN_FLEX: 
	rm -Rf $(FLEX_SRC)

CLEAN_BISON:
	rm -Rf $(BISON_SRC) $(BISON_HEADERS)
