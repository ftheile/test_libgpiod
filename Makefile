app = test_libgpiod
src = $(wildcard *.c)
obj = $(src:.c=.o)
dep = $(obj:.o=.d)  # one dependency file for each source

CFLAGS = -Wall -Wextra `pkg-config --cflags libgpiod`
LDFLAGS = `pkg-config --libs libgpiod`

.PHONY: all
all: $(app)

$(app): $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(dep)   # include all dep files in the makefile

# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) $(app)

.PHONY: cleandep
cleandep:
	rm -f $(dep)
