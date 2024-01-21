CC = gcc

SRC_DIR = ./src

DAI_SRC_DIR = DAI
DAI_SOURCES = $(wildcard $(SRC_DIR)/$(DAI_SRC_DIR)/*.c)
# DAI_SOURCES = $(addprefix $(DAI_SRC_DIR)/,$(DAI_FILES))

DAI_IDIR = ./include/DAI
DAI_IFILES = $(wildcard $(DAI_IDIR)/*.h)

SRC = $(wildcard $(SRC_DIR)/*.c) $(DAI_SOURCES)

ODIR = $(SRC_DIR)/obj
OBJ = $(SRC:$(SRC_DIR)/%.c=$(ODIR)/%.o)

OUT_DIR = ./bin
OUT = $(OUT_DIR)/main.exe
OUT_DB = $(OUT_DIR)/main_db.exe
OUT_GMP = $(OUT_DIR)/main_gmp.exe

IDIR = ./include
IDIRS = $(IDIR) $(DAI_IDIR)
IFLAGS = $(addprefix -I,$(IDIRS))
IFILES = $(wildcard $(IDIR)/*.h) $(DAI_IFILES)

CUSTOM_ODIR = $(ODIR)/custom
CUSTOM_OBJ = $(OBJ:$(ODIR)/%.o=$(CUSTOM_ODIR)/%.o)

GMP_FLAGS = -lgmp -D__DAI_USE_GMP__
GMP_ODIR = $(ODIR)/gmp
GMP_OBJ = $(OBJ:$(ODIR)/%.o=$(GMP_ODIR)/%.o)

SPECIAL_FLAGS = -O3
STD_FLAGS = -Wall -Wextra -pedantic $(IFLAGS)
CFLAGS = $(STD_FLAGS) $(SPECIAL_FLAGS)

run: $(OUT)
	./$^

db: $(OUT_DB) Makefile $(IFILES)
	gdb ./$<

gmp: set_gmp $(OUT_GMP)
	./$(OUT_GMP)

set_gmp:
	$(eval CFLAGS += $(GMP_FLAGS))

$(OUT) : $(CUSTOM_OBJ)
$(OUT_GMP): $(GMP_OBJ)

$(OUT) $(OUT_GMP): | $(OUT_DIR)
	$(CC) $^ -o $@ $(CFLAGS)

$(CUSTOM_ODIR)/%.o $(GMP_ODIR)/%.o: $(SRC_DIR)/%.c Makefile $(IFILES) | $(ODIR) $(CUSTOM_ODIR) $(GMP_ODIR) $(CUSTOM_ODIR)/DAI $(GMP_ODIR)/DAI
	$(CC) $< -c -o $@ $(CFLAGS)

$(OUT_DIR) $(ODIR) $(CUSTOM_ODIR) $(GMP_ODIR) $(CUSTOM_ODIR)/DAI $(GMP_ODIR)/DAI $(LATEX_AUX_DIR):
	mkdir -p $@

$(OUT_DB): $(SRC)
	$(CC) $^ -o $@ $(STD_FLAGS) -ggdb
