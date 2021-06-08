

SDIR := ./src
IDIR := ./inc
ADIR := ./app
TDIR := ./test

SRC := $(wildcard $(SDIR)/*.c)

ASRC := $(SRC) $(wildcard $(ADIR)/*.c)
TSRC := $(SRC) $(wildcard $(TDIR)/*.c)

AOBJ := $(ASRC:%.c=%.o)
TOBJ := $(TSRC:%.c=%.o)

OBJ := $(AOBJ) $(TOBJ)

DEPS := $(OBJ:%.o=%.d)


EXEC := main.out
TEXEC := test.out

CC ?= gcc

H_INC := $(foreach d, $(IDIR), -I$d)

C_FLAGS := -Wall -Wextra

DEP_FLAGS := -MMD -MP



ifeq ($(CC),clang)
	C_FLAGS += -Weverything
else ifneq (, $(filter $(CC), cc gcc))
	C_FLAGS += -rdynamic
endif

ifeq ("$(origin O)", "command line")
	OPT := -O$(O)
else
	OPT := -O3
endif

C_FLAGS += $(OPT) $(DEP_FLAGS)

all: $(EXEC)

test: $(TEXEC)

$(EXEC): $(AOBJ)
	$(CC) $(C_FLAGS) $(H_INC) $(AOBJ) -o $@

$(TEXEC): $(TOBJ)
	$(CC) $(C_FLAGS) $(H_INC) $(TOBJ) -o $@

%.o:%.c %.d
	$(CC) $(C_FLAGS) $(H_INC) -c $< -o $@

clean:
	rm -rf $(EXEC)
	rm -rf $(TEXEC)
	rm -rf $(OBJ)
	rm -rf $(DEPS)


$(DEPS):

include $(wildcard $(DEPS))

