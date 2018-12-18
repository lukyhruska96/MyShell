SRC_DIR = src
FLEX_FILE := $(SRC_DIR)/lexer.lex
FLEX_SRC := $(FLEX_FILE:.lex=.c)

all: flex

flex: $(FLEX_SRC)


$(FLEX_SRC) : %.c : %.lex
	flex -o $@ $<

clean: CLEAN_FLEX

CLEAN_FLEX: 
	rm -Rf $(FLEX_SRC)
