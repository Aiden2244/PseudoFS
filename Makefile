CC = gcc
CFLAGS = -std=c11 -Wall -D_XOPEN_SOURCE=700
LIBS = -lrt

LIB_DIR = libPseudoFS
LIB_NAME = libPseudoFS.a

TEST_DIR = test
PROG_DIR = programs
USER_DIR = user

SRC_FILES = $(wildcard $(LIB_DIR)/*.c)
OBJ_FILES = $(patsubst %.c,%.o,$(SRC_FILES))

TEST_SRC_FILES = $(wildcard $(TEST_DIR)/*.c)
TEST_PROGS = $(patsubst %.c,%.out,$(TEST_SRC_FILES))

PROG_SRC_FILES = $(wildcard $(PROG_DIR)/*.c)
PROGS = $(patsubst %.c,%.out,$(PROG_SRC_FILES))

.PHONY: all
all: $(LIB_DIR)/$(LIB_NAME) $(TEST_PROGS) $(PROGS) $(USER_DIR)/pdos_mkdisk $(USER_DIR)/pdos_mkfs

$(LIB_DIR)/$(LIB_NAME): $(OBJ_FILES)
	ar rcs $@ $^

$(TEST_DIR)/%.out: $(TEST_DIR)/%.c $(LIB_DIR)/$(LIB_NAME)
	$(CC) $(CFLAGS) $^ -o $@ -L$(LIB_DIR) -lPseudoFS $(LIBS)

$(PROG_DIR)/%.out: $(PROG_DIR)/%.c $(LIB_DIR)/$(LIB_NAME)
	$(CC) $(CFLAGS) $^ -o $@ -L$(LIB_DIR) -lPseudoFS $(LIBS)

$(USER_DIR)/pdos_mkdisk: $(USER_DIR)/pdos_mkdisk.c $(LIB_DIR)/$(LIB_NAME)
	$(CC) $(CFLAGS) $^ -o $@ -L$(LIB_DIR) -lPseudoFS $(LIBS)

$(USER_DIR)/pdos_mkfs: $(USER_DIR)/pdos_mkfs.c $(LIB_DIR)/$(LIB_NAME)
	$(CC) $(CFLAGS) $^ -o $@ -L$(LIB_DIR) -lPseudoFS $(LIBS)

.PHONY: clean
clean:
	rm -f $(LIB_DIR)/*.o $(LIB_DIR)/$(LIB_NAME) $(TEST_DIR)/*.o $(TEST_DIR)/*.out $(PROG_DIR)/*.o $(PROG_DIR)/*.out $(USER_DIR)/pdos_mkdisk $(USER_DIR)/pdos_mkfs
