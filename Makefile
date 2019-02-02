SUBDIRS := $(wildcard src/*/.)

all: $(SUBDIRS)
$(SUBDIRS):
	@[ -f $@/Makefile ] && $(MAKE) -C $@ || echo "WARNING: Nao Existe Makefile \"$@/Makefile\""
clean:
	find bin/. ! -name 'readme.md' -type f -exec rm -f {} +
.PHONY: $(TOPTARGETS) $(SUBDIRS)
