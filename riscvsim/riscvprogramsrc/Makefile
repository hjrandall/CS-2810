.SUFFIXES:
.SUFFIXES: .o .s

ASFLAGS=-ggdb --warn --fatal-warnings -march=rv32im -mno-relax
LDFLAGS=--fatal-warnings -melf32lriscv -Ttext 1074
PREFIX=riscv64-unknown-elf
RUN=qemu-riscv32
ASSEMBLER=$(shell which $(PREFIX)-as)

ALLOBJECT=$(sort $(patsubst %.s,%.o,$(wildcard *.s))) $(sort $(patsubst %.s,%.o,$(wildcard lib/*.s)))
START=$(filter start.o, $(ALLOBJECT))
AOUTOBJECT=$(START) $(filter-out $(START), $(ALLOBJECT))

all:	dist

.s.o:
ifeq ("$(ASSEMBLER)", "")
	$(error this should only be run on the cs2810.cs.dixie.edu server)
endif
	$(PREFIX)-as $(ASFLAGS) $< -o $@

../riscvprogram:	$(AOUTOBJECT)
	$(PREFIX)-ld $(LDFLAGS) $^ -o $@
	$(PREFIX)-strip $@

dist:	../riscvprogram
	rm -f *.o core

clean:
	rm -f *.o ../riscvprogram core
