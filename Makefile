CC = gcc

SRC_DIR = ./src

DAI_SRC_DIR = DAI
DAI_SOURCES = $(wildcard $(SRC_DIR)/$(DAI_SRC_DIR)/*.c)
# DAI_SOURCES = $(addprefix $(DAI_SRC_DIR)/,$(DAI_FILES))

DAI_IDIR = ./include/DAI
DAI_IFILES = $(wildcard $(DAI_IDIR)/*.h)

SRC = $(wildcard $(SRC_DIR)/*.c) $(DAI_SOURCES)

ODIR = $(SRC_DIR)/obj
# OBJ = $(addprefix $(ODIR)/,$(SRC_FILES:.c=.o))
OBJ = $(SRC:$(SRC_DIR)/%.c=$(ODIR)/%.o)

OUT_DIR = ./bin
OUT = $(OUT_DIR)/main.exe
OUT_DB = $(OUT_DIR)/main_db.exe

IDIR = ./include
IDIRS = $(IDIR) $(DAI_IDIR)
IFLAGS = $(addprefix -I,$(IDIRS))
IFILES = $(wildcard $(IDIR)/*.h) $(DAI_IFILES)

LATEX_AUX_DIR=./latex
LATEX_SOURCE=./latex/computation_steps.tex

SPECIAL_FLAGS = -O3
STD_FLAGS = -Wall -Wextra -pedantic $(IFLAGS)
CFLAGS = $(STD_FLAGS) $(SPECIAL_FLAGS)

run: $(OUT)
	./$^

db: $(OUT_DB) Makefile $(IFILES)
	gdb ./$<

$(OUT): $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS)

$(OUT_DB): $(SRC)
	$(CC) $^ -o $@ $(STD_FLAGS) -ggdb

$(ODIR)/%.o:$(SRC_DIR)/%.c Makefile $(IFILES)
	$(CC) $< -c -o $@ $(CFLAGS)

latex: $(LATEX_SOURCE) Makefile
	pdflatex -aux-directory=$(LATEX_AUX_DIR) $(LATEX_SOURCE) --interaction=batchmode