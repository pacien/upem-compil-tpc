# UPEM / Compilation / Projet
# Pacien TRAN-GIRARD, Adam NAILI

# DIRECTORIES
SRC_DIR := src
RES_DIR := res
OUT_DIR := out
DOC_DIR := doc

# INPUT
LEX_SRC := tpc.lex
YACC_SRC := tpc.y
PDF_SRC := rapport.md
ST_SRC := symbol_table

# INTERMEDIATE
LEX_GEN := tpc.yy
YACC_GEN := tpc.tab

# OUTPUT
COMPIL_BIN := tcompil
COMMIT_LOG := commits.log
REPORT_PDF := rapport.pdf

# CC PARAMS
CC := gcc
LEX := flex
YACC := bison
ASM := nasm
AFLAGS := -f elf64
CFLAGS := -ansi -pedantic -Wall -std=gnu99 -O2
LFLAGS := -lfl -ly
IFLAGS += -I$(SRC_DIR) -I$(OUT_DIR)

# RULES
all: clean $(OUT_DIR)/$(COMPIL_BIN)

test: $(OUT_DIR)/$(FILE_TEST).o
	$(CC) $< -o $(OUT_DIR)/$(FILE_TEST) -nostartfiles

$(OUT_DIR)/$(FILE_TEST).o: $(OUT_DIR)/$(FILE_TEST).asm
	$(ASM) $(AFLAGS) $< -o $@

$(OUT_DIR)/$(FILE_TEST).asm: $(RES_DIR)/$(FILE_TEST).tpc $(OUT_DIR)/$(COMPIL_BIN)
	$(OUT_DIR)/$(COMPIL_BIN) < $< > $@

$(OUT_DIR)/$(LEX_GEN).c: $(SRC_DIR)/$(LEX_SRC)
	$(LEX) -o $@ $^

$(OUT_DIR)/$(YACC_GEN).c $(OUT_DIR)/$(YACC_GEN).h: $(SRC_DIR)/$(YACC_SRC) $(SRC_DIR)/$(ST_SRC).h
	$(YACC) --output=$(OUT_DIR)/$(YACC_GEN).c --defines=$(OUT_DIR)/$(YACC_GEN).h -v $<

$(OUT_DIR)/$(LEX_GEN).o: $(OUT_DIR)/$(LEX_GEN).c $(OUT_DIR)/$(YACC_GEN).h
	$(CC) -o $@ -c $< $(IFLAGS) $(LFLAGS) $(CFLAGS)

$(OUT_DIR)/$(YACC_GEN).o: $(OUT_DIR)/$(YACC_GEN).c
	$(CC) -o $@ -c $^ $(IFLAGS) $(LFLAGS) $(CFLAGS)

$(OUT_DIR)/$(ST_SRC).o: $(SRC_DIR)/$(ST_SRC).c
	$(CC) -o $@ -c $^ $(IFLAGS) $(LFLAGS) $(CFLAGS)

$(OUT_DIR)/$(COMPIL_BIN): $(OUT_DIR)/$(LEX_GEN).o $(OUT_DIR)/$(YACC_GEN).o $(OUT_DIR)/$(ST_SRC).o
	$(CC) -o $@ $^ $(IFLAGS) $(LFLAGS) $(CFLAGS)

$(OUT_DIR)/$(REPORT_PDF): $(DOC_DIR)/$(PDF_SRC)
	pandoc --template $(DOC_DIR)/report-template.tex --number-sections --listings --output $@ $^

$(OUT_DIR)/$(COMMIT_LOG):
	git log > $@

clean:
	rm -rf $(OUT_DIR)/*
